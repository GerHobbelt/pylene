Cleaning iterators
##################

Histoire
========

Les traitements de base en TI s'expriment généralement sous deux formes:

1. Pour chaque pixels, effectuer une action. Par exemple, un changement de
   contraste `g` s'écrira::


     void change_contrast(int* buffer, std::size_t width, std::size_t height, std::size_t stride)
     {
       auto g = [](auto x) { return x + 1; };
       for (std::size_t y = 0; y < height; ++y)
       {
         auto lineptr = buffer + y * stride;
         for (std::size_t x = 0; x < width; ++x)
           lineptr[x] = g(lineptr[x]);
       }
     }

2. Pour chaque pixels, pour chaque voisin, effectuer une action. Par example,
   un filtre moyenneur 3x3 s'écrira (on omet les problèmes d'arrondis et de bordure)::

     void boxcar(const int* __restrict inbuffer, int* __restrict outbuffer, std::size_t width, std::size_t height, std::size_t stride)
     {
       for (std::size_t y = 0; y < height; ++y)
       {
         auto lineptr = inbuffer + y * stride;
         auto outptr = outbuffer + y * stride;
         for (std::size_t x = 0; x < width; ++x)
         {
            int sum = 0;
            auto ptr = lineptr + x;
            for (int dy = -1; dy <= 1; ++dy)
                for (int dx = -1; dx <= 1; ++dx)
                    sum += ptr[dy * stride + dx];
            outptr[x] = sum / 9;
         }
       }
     }

Pour le premier code, tous les compilateurs (GCC, CLANG, MSVC, ICC) sont
capables de générer du code optimisé (vectorisé). Pour le second, il faut aider
le compilateur pour lui dire que les pointeurs ne sont pas aliasés. Mais même
alors, MSVC échoue à vectoriser. Si on déroule la boucle manuellement, alors il
y arrive.::

  void boxcar_unrolled(const int* __restrict inbuffer, int* __restrict outbuffer, std::size_t width, std::size_t height, std::size_t stride)
  {
    for (std::size_t y = 0; y < height; ++y)
    {
        auto prevlineptr = inbuffer + (y-1) * stride;
        auto nextlineptr = inbuffer + (y+1) * stride;
        auto lineptr = inbuffer + y * stride;
        auto outptr = outbuffer + y * stride;
        for (std::size_t x = 0; x < width; ++x)
        {
            auto sum1 = prevlineptr[x-1] + prevlineptr[x] + prevlineptr[x+1];
            auto sum2 = lineptr[x-1] + lineptr[x] + lineptr[x+1];
            auto sum3 = nextlineptr[x-1] + nextlineptr[x] + nextlineptr[x+1];
            outptr[x] = (sum1 + sum2 + sum3) / 9;
        }
    }
   }



On se rend compte alors que cette écriture n'est pas ce que l'on veut:

* elle est non-générique (image 3D ?)
* elle est difficile à écrire et sujette à l'erreur

En revanche:

* elle permet d'obtenir un code rapide

Les itérateurs ont été introduits pour permettre une abstraction. Le code
suivant est le même pour le 2D et le 3D::

  void change_constrat(image2d& f)
  {
    for (auto p : f.domain())
      f(p) = f(p) + 1;
  }

  void boxcar(const image2d& f, image2d& g)
  {
    for (auto p : f.domain())
    {
      int sum = 0;
      for (auto n : win3x3(p))
        sum += f(n);
      g(p) = sum / 9;
    }
  }


Le code est élégant mais l'asm produit horrible.

Design du code actuel et problèmes
==================================

L'itérateur sur pixel
---------------------

Les positions sont recalculées à chaque fois. `f(p) === f.buffer[p.y * f.stride + p.x]`.

* lors d'une itération externe, les pointeurs/indexes sont de simplement décalés
* lors d'une itération interne, les pointeurs/indexes sont des offsets par
  rapport à la position courante.

(noter que pour le compilateur `f.buffer[?]` peut aliaser `f.stride` s'ils sont
du même type).

Les itérateurs sur pixels ont été introduits dans cet objectif. Ils fournissent
à la fois la valeur et la position dans l'image (`px.point()`, `px.val()`)::

  void change_constrat(image2d& f)
  {
    for (auto px : f.pixels())
      px.val() = px.val() + 1;
  }

  void boxcar(const image2d& f, image2d& g)
  {
    for (auto [pxin, pxout] : ziprange(f.pixels(), g.pixels()))
    {
      int sum = 0;
      for (auto nx : win3x3(pxin))
         sum += nx.val();
      nxout = sum / 9;
    }
  }

Çà aurait pu être beau, mais attention à ce que fait `win3x3(pxin)`, il utilise
la position de `pxin` et créé un tableau d'offset. Si le compilateur n'est pas
capable de produire une "Loop Invariant Code Motion" de cette opération, il n'y
a aucun bénéfice. Aucun des compilateurs n'a été capable de le produire à
l'époque. On déplace donc la création des itérateurs à l'extérieur de la
boucle avec des itérateurs qui se *bind* sur d'autres itérateurs.::

  void boxcar(const image2d& f, image2d& g)
  {
    auto&& inrng = f.pixels();
    auto&& outrng = g.pixels();
    auto pxin = std::begin(inrng);
    auto pxend = std::end(inrng);
    auto pxout = std::begin(outrng);
    auto nbh = win3x3(pxin); // <- bounded to pxin

    for(; pxin != pxend; ++pxin, ++pxend)
    {
      int sum = 0;
      for (auto&& nx : nbh)
         sum += nx.val();
      pxout->val() = sum / 9;
    }
  }


Et le code devient lourd. C'est dû au fait que les itérateurs en C++ sont
flexibles mais lourd dans l'écriture. Les itérateurs ne connaissent ni leur
début, ni leur fin. On a donc pallié à cela::

  class iterator
  {
    void init();
    void next();
    bool is_finished() const;
    T dereference() const;
  }

  void boxcar(const image2d& f, image2d& g)
  {
    mln_iter(pxin, f.pixels());
    mln_iter(pxout, g.pixels());
    mln_iter(nx, win3x3(pxin));

    mln_forall(pxin, pxout)
    {
      int sum = 0;
      mln_forall(nx)
         sum += nx->val();
      pxout->val() = sum / 9;
    }
  }

Mais ce n'est pas rose:

* Incompatibilité avec la STL
* Des macros qui remplacent les "for"
* Et toujours des perfs manquantes... (whattt ??? )


Cas de la simple boucle
-----------------------

Pour permettre d'écrire ce code, des boucles imbriquées doivent être réécrites
en simple boucle. Un objet `nditerator` permet de faire un *flatten*.::

    void change_constrat(image2d& f)
    {
      mln_foreach(auto px, f.pixels())
        px.val() = px.val() + 1;
    }


Avec `mln_foreach(X, RANGE)`. ::

  {
     auto it = RANGE.iter()
     for (it.init(); !it.is_finished(); it.next())
     {
       X = it.dereference();
       ...
     }
  }

Cependant `it.next()` n'est pas trivial. C'est à cause de ça que les compilateurs ont du
mal à vectoriser. Typiquement::

  void iterator2d::next()
  {
    x++;
    if (x == xend)
    {
      y++;
      x = 0;
    }
  }


C'est pourquoi une première optimisation a été que `mln_forall` et `mln_foreach`
soient deux boucles par défaut. `mln_foreach(X, RANGE)`::

  {
    auto it = RANGE.iter()
    for (it.outer_init(); !it.outer_is_finished(); it.outer_next())
      for (it.inner_init(); !it.inner_is_finished(); it.inner_next())
      {
        X = it.dereference();
        ...
      }
  }

Grâce à cela, on arrive à récupérer une performance proche du C mais le coût
est élevé:

* Non conforme au standard
* Perte de la compatibilité avec la STL
* Beaucoup de code non-visible du point de vue *end-user*
* Beaucoup de code tricky => trés mauvais pour la maintenance !!


Cas de la boucle imbriquée
--------------------------

On est loin de la performance du C. Le fait que le voisinage tienne une
back-référence vers le pixel itérateur empêche la SROA sur certains
compilateurs.

De plus, les problèmes d'aliasing d'images font que le code n'est pas
vectorisé. Le `__restrict` sur les pointeurs dans les `struct` ne sont pas
toujours compris.




Solution possible
=================

Les co-routines permettent de créer des générateurs n-dimentionnel à moindre
coût avec une SROA assuré. On espère:

* Un gain en perf
* Une réécriture conforme aux concepts du standard
* Du code grandement simplifié















