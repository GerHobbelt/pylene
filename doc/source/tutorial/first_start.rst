First steps in the library
==========================


Objectives:

* provide image types and basic manipulation routines
* provide a framework for easy-to-write and efficient image processing algorithms

Image representation in Pylene
------------------------------

An :cpp:concept:`Image` is basically a function associating a *point* to a
*value*. The pair *(point,value)* is called a :cpp:concept:`Pixel`. The *domain* of
an image is the *ordered range* of points for which the function is
defined.

* Example of *point* type:
  ``mln::point2d``, ``mln::point3d``...
* Example of *domain* types:
  ``mln::box2d``, ``std::set<mln::point2d>``
* Example of *value* types:
  ``uint8_t``, ``double``, ``mln::rgb8``
* Example of *image* types:
  ``mln::image2d<int>``, ``mln::image3d<uint8>``





Creating/Loading/Saving an image
--------------------------------

Creating an image from scratch
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

:cpp:class:`mln::image2d` is the most-used image type representing images
defined on a 2D grid. It can be creating from a set of values. For
instance, in the following code, a 2 × 3 2D image is created with
values from 1 to 6::

  #include <mln/core/image/ndimage.hpp>
  mln::image2d<int> f = {{1,2,3}, {4,5,6}};

A 2D image can also be created from its width and height::

  mln::image2d<int> f(2,3);

Finally a 2D image domain may not start at `(0,0)`. One can initialize
an image from its domain and also provide an initial value for its
elements (and also a border, we will see later)::

  mln::box2d d = {{1,1}, {5,5}};
  mln::image2d<uint8_t> f(d);       // A 4 x 4 image starting at (1,1) with a default border
  mln::image2d<uint8_t> g(d, 0);    // A 4 x 4 image starting at (1,1) with no border
  mln::image2d<uint8_t> h(d, 0, 3); // A 4 x 4 image starting at (1,1) with no border initialized with value 3


Loading an image
^^^^^^^^^^^^^^^^

An image can be loaded from a file. It actually relies on the
`freeimage <http://freeimage.sourceforge.net/>`_ library to
handle most file formats::

  #include <mln/core/image/ndimage.hpp>
  #include <mln/io/imread.hpp>

  mln::image2d<uint8_t> f;
  mln::io::imread("input.pgm", f);

Saving an image
^^^^^^^^^^^^^^^

An image can be saved to a file. Yet it relies on the `freeimage
<http://freeimage.sourceforge.net/>`_. The file format is
automatically deduced from the filename extension when it is
possible::

  #include <mln/core/image/ndimage.hpp>
  #include <mln/io/imsave.hpp>

  mln::image2d<uint8_t> f = { {1,2,3}, {4,5,6} };
  mln::io::imsave(f, "out.pgm");


Displaying an image
^^^^^^^^^^^^^^^^^^^

To display an image on the console, use :cpp:func:`mln::io::imprint`::

  #include <mln/core/image/ndimage.hpp>
  #include <mln/io/imprint.hpp>

  mln::image2d<uint8_t> f = { {1,2,3}, {4,5,6} };
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

  mln::image2d<uint8_t> f = { {1,2,3}, {4,5,6} };
  mln::point2d p = {1,1};
  std::cout << f(p) << "\n"; // Affiche 5

Note that indexes are in the order `(row, col)`. `(0,0)` refers to the
topleft corner and `(1,2)` to the bottom-right corner.

For 2D image, an equivalent writing is with :cpp:func:`image2d<T>::at_`::

  mln::image2d<uint8_t> f = { {1,2,3}, {4,5,6} };
  std::cout << f.at_(1,1) << "\n"; // Affiche 5

.. note::

   There are some first differences with Milena_:

   * First the range for the domain in Milena_ is closed,
     (i.e. `domain = [pmin,pmax]`), here it is half-open to follow the
     usual convention of the C++ standard library, so it is `domain =
     [pmin,pmax)` and `pmax` is not included in the range.

   * Second, every image ``f`` in Milena_ supports ``f(p)`` where
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

.. literalinclude:: /snippets/first_start_1.cpp

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

   Contrary to Milena_, the ordering of the traversal of images is
   defined. Domains are *ordered* ranges. This means there is a
   **total order** on *point* type. In the case of
   :cpp:class:`mln::point2d`, this is the lexicographical order
   (i.e., the natural scan order of 2D grids). Traversing with *value*
   or *pixel* range follows the same order.


Traversing with ranges and iterators
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The underlying concepts behind multidimensional ranges. They are like
normal C++ ranges but have a different interface to enable faster traversal. This is why, you
cannot use a normal `for` on these ranges but ``mln_foreach``.


  mln_foreach(auto px, f.pixels())
    std::cout << px.point() << " : ' << px.val() << "\n";

.. note::

   Contrary to Milena_ with do not provide *proxy iterators* as it is
   more common to directly iterate with ``mln_foreach`` on values.



Traversing several images in the same time
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Image and range views enable to pack (zip) several objects in a single object::

  auto vals_1 = f.values();
  auto vals_2 = g.values();
  mln_foreach((auto [v1, v2]), mln::ranges::view::zip(vals_1, vals_2))
    ...

You can also iterate using a domain is the image supports direct access by points::

  mln_foreach(auto p, domain)
    // Use f(p) and f(g)



























