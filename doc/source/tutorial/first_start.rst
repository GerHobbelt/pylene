First steps in the library
==========================


Objectives:

* provide image types and basic manipulation routines
* provide a framework for easy-to-write and efficient image processing algorithms

Foundamental image concepts
---------------------------

An image is basically a function associating a *point* to a
*value*. The pair *(point,value)* is called a *pixel*. The *domain* of
an image is the *ordered range* of points for which the function is
defined.

* Example of *point* type:
  ``point2d``, ``point3d``...
* Example of *domain* types:
  ``box2d``, ``sbox2d`` (strided boxes)
* Example of *value* types:
  ``mln::uint8``, ``double``, ``mln::rgb8``
* Example of *image* types:
  ``image2d<int>``, ``image3d<uint8>``
* Example of *pixel* types:
  ``image2d<int>::pixel_type``

Creating/Loading/Saving an image
--------------------------------

Creating an image from scratch
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

:cpp:class:`mln::image2d` is the most-used image type representing images
defined on a 2D grid. It can be creating from a set of values. For
instance, in the following code, a 2 × 3 2D image is created with
values from 1 to 6::

  #include <mln/core/image/image2d.hpp>
  mln::image2d<int> f = {{1,2,3}, {4,5,6}};

A 2D image can also be created from its number of rows and columns::

  mln::image2d<int> f(2,3);

Finally a 2D image domain may not start at `(0,0)`. One can initialize
an image from its domain and also provide an initial value for its
elements (and also a border, we will see later)::

  mln::box2d d {{1,1}, {5,5}};
  mln::image2d<mln::uint8> f(d);       // A 4 x 4 image starting at (1,1) with a default border
  mln::image2d<mln::uint8> g(d, 0);    // A 4 x 4 image starting at (1,1) with no border
  mln::image2d<mln::uint8> h(d, 0, 3); // A 4 x 4 image starting at (1,1) with no border initialized with value 3


Loading an image
^^^^^^^^^^^^^^^^

An image can be loaded from a file. It actually relies on the
`freeimage <http://freeimage.sourceforge.net/>`_ library to
handle most file formats::

  #include <mln/core/image/image2d.hpp>
  #include <mln/io/imread.hpp>

  mln::image2d<mln::uint8> f;
  mln::io::imread("input.pgm", f);

Saving an image
^^^^^^^^^^^^^^^

An image can be saved to a file. Yet it relies on the `freeimage
<http://freeimage.sourceforge.net/>`_. The file format is
automatically deduced from the filename extension when it is
possible::

  #include <mln/core/image/image2d.hpp>
  #include <mln/io/imsave.hpp>

  mln::image2d<mln::uint8> f = { {1,2,3}, {4,5,6} };
  mln::io::imsave(f, "out.pgm");


Displaying an image
^^^^^^^^^^^^^^^^^^^

To display an image on the console, use :cpp:func:`mln::io::imprint`::

  #include <mln/core/image/image2d.hpp>
  #include <mln/core/grays.hpp>
  #include <mln/io/imprint.hpp>

  mln::image2d<mln::uint8> f = { {1,2,3}, {4,5,6} };
  mln::io::imprint(f);


It should display.

.. code-block:: none

   [[0,0] ... [2,3]](h)
   1 2 3 
   4 5 6 


Traversing and accessing image values
-------------------------------------

Accessing image values
^^^^^^^^^^^^^^^^^^^^^^

Most standard image types (see :concept:`Accessible Image`) supports
accessing a value though its point::

  mln::image2d<mln::uint8> f = { {1,2,3}, {4,5,6} };
  point2d p {1,1};
  std::cout << f(p) << "\n"; // Affiche 5

Note that indexes are in the order `(row, col)`. `(0,0)` refers to the
topleft corner and `(1,2)` to the bottom-right corner.

For 2D image, an equivalent writing is with :cpp:func:`image2d<T>::at_`::

  mln::image2d<mln::uint8> f = { {1,2,3}, {4,5,6} };
  std::cout << f.at_(1,1) << "\n"; // Affiche 5

.. note::

   There are some first differences with |milena|:

   * First the range for the domain in |milena| is closed,
     (i.e. `domain = [pmin,pmax]`), here it is half-open to follow the
     usual convention of the C++ standard library, so it is `domain =
     [pmin,pmax)` and `pmax` is not included in the range.

   * Second, every image ``f`` in |milena| supports ``f(p)`` where
     ``p`` is point of the domain. This is not always the case with
     this library as we provide more flexible ways to iterate on images.

Traversing images
^^^^^^^^^^^^^^^^^

Most images (more precisely :concept:`Forward Image`) provide 3 ways
of traversing.

* Iterating on *points* through the *domain*
* Iterating on *values*
* Iterating on *pixels*

As a consequence, such an image ``f`` provide:

* ``f.domain()`` The range of the image *points*
* ``f.values()`` The range of the image *values*
* ``f.pixels()`` The range of the image *pixels*

An image can be birectional (see :concept:`Bidirectional Image`) and
allows to be traversed in the reverse order. In that case, the
*ranges* are bidirectional as well. Below is an example of image traversal.

.. literalinclude:: /snipsets/first_start_1.cpp

It produces the following output.

.. code-block:: none

   == Traversing forward ==
   Traversing through points.
   [0,0]:1
   [0,1]:2
   [0,2]:3
   [1,0]:4
   [1,1]:5
   [1,2]:6
   Traversing on values.
   1
   2
   3
   4
   5
   6
   Traversing with pixels.
   [0,0]:1
   [0,1]:2
   [0,2]:3
   [1,0]:4
   [1,1]:5
   [1,2]:6

   == Traversing backward ==
   Traversing through points.
   [1,2]:6
   [1,1]:5
   [1,0]:4
   [0,2]:3
   [0,1]:2
   [0,0]:1
   Traversing on values.
   6
   5
   4
   3
   2
   1
   Traversing with pixels.
   [1,2]:6
   [1,1]:5
   [1,0]:4
   [0,2]:3
   [0,1]:2
   [0,0]:1

.. note::

   Contrary to |milena|, the ordering of the traversal of images is
   defined. Domains are *ordered* ranges. This means there is a
   **total order** on *point* type. In the case of
   :cpp:class:`mln::point2d`, this is the lexicographical order
   (i.e., the natural scan order of 2D grids). Traversing with *value*
   or *pixel* range follows the same order.


Traversing with iterators
^^^^^^^^^^^^^^^^^^^^^^^^^

The underlying concepts behind *ranges* are **iterators**. A range `R`
provides iterators (and sometimes reverse iterators) through
``R.iter()`` (and ``R.riter()``). Then, the macro ``mln_forall(it1,
it2,...)`` allows to iterate. The element pointed by the iterator is
accessed with ``*it`` as usually::

  auto&& pxrng = f.pixels();
  auto px = pxrng.iter();
  mln_forall(px)
    std::cout << px->point() << " : ' << px->val() << "\n";

Note the use ``auto&& pxrng`` in order to extend the life of range if
its a temporary (nothing says that the *pixels range* is a reference!).
To avoid this burden, we provide some helper macros:

* ``mln_pixter(px, f)`` declares a pixel iterator to ``f.pixels()``
* ``mln_viter(v, f)`` declares a value iterator to ``f.values()``
* ``mln_piter(p, f)`` declares a point iterator to ``f.domain()``

The previous code can be simplified by::

  mln_pixter(px, f);
  mln_forall(px)
    std::cout << px->point() << " : ' << px->val() << "\n";


.. note::

   Contrary to |milena| with do not provide *proxy iterators* as it is
   more common to directly iterate with ``mln_foreach`` on values.



Traversing several images in the same time
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The direct solution would be::

  mln_pixter(px1, f);
  mln_pixter(px2, g);
  mln_forall(px1, px2)
    ...

However, if `f` and `g` have similar structure (e.g. same domain, same
strides), it leads to redoundant computation (bound checking of rows
is performed twice,...). The framework allows to declare several
iterators in the same time that **must** be used **jointly in a forall
loop**. You should never use then in an other case, as they are actually
*binded*, incrementing one would increment the other. The syntax is::

  mln_pixter(px1, px2, f, g);
  mln_forall(px1, px2)
    ...


``mln_pixter`` and ``mln_viter`` are actually variadic and allows to iterate on
how many images you want e.g. ``mln_pixter(px₁, px₂, ..., pxₖ, f₁, f₂, ..., fₖ)``.




























