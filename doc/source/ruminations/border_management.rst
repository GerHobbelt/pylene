Taxonomie des algorithmes
#########################

* Global
* Local (Stencil)
* Point-wise (Map)

Role du Border Manager
======================

Soit:

* `WR` la ROI de travail, sur laquelle on souhaite obtenir le résultat de l'opérateur
* `IR` la ROI d'input, dont les valeurs sont nécéssaires pour calculer les valeurs de `OR`.

Dans le cas d'un opérator local, les deux valeurs sont liées par:

* ``IR = se.compute_input_region(WR)``
* ``WR = se.compute_output_region(IR)``

Génerateur d'input
******************

Le border manager à alors pour rôle de faire en sorte qu'une image d'entrée soit *lisible* sur `IR`
en laissant à l'utilisateur la possiblité de configurer la façon dont les valeurs de IR. Les configurations
possibles de gestion sont:

* user: les données proviennent de l'extension de l'image, gérées par l'utilisateur
* fill(v):  les valeurs prendront toujours la valeur `v`
* periodize: En 2D, ``managed(x,y) = f(x % N, x % M)`` 
* mirror: En 2D: ``managed(x) = f(N - |(x % 2N) - N|, M - |y % 2M - M|)`` 

A noter que tous les modes ne sont pas compatibles avec toutes les images.

Lorsqu'on éxécute ``bm.manage(ima, se, WR)``, l'algorithme suivant est appliqué.

#. Si l'image d'entrée possède une extension, et si l' extension est suffisament large (``extension.includes(IR)``), et
   supporte le *mode* (``extension::suport_**mode**``), alors en fonction de *mode*, on éxécute:

   * *rien* si *mode = user*
   * ``extension.fill(v)`` si *mode = fill*
   * ``extension.periodize()`` si *mode = periodize*
   * ``extension.mirror()`` si *mode = mirror*

#. Sinon si, le mode n'est pas *user* et que la gestion est *auto*, une image avec une extension artificielle est
   ajoutée en fonction du mode:

   * view::value_extended
   * view::periodize_extended
   * view::mirror_extended

#. Sinon, c'est une erreur.

Generateur d'image temporaire
*****************************

Certains algorithmes ont besoin d'images temporaires qui seront écrites. Le BM doit donc être capable de créer une
nouvelle image avec des valeurs d'extension matérialisée en mémoire::

    output = bm.create_temporary(ima, se, WR)

Crée une image dont le domaine sera ``IR = se.compute_input_region(WR)`` et dont les valeurs étendues seront calculées en fonction du mode.


Role du canvas algorithmique local
==================================

Il y a deux cas à penser:
df

