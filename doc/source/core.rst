Core Module
===========




.. toctree::
   :hidden:


   core/images
   core/ranges
   core/neighborhood
   core/stl









.. raw:: html

         <h2>Images</h2>


See :doc:`core/images` for a description of the image concepts and image basics.


.. topic:: Utilities

  .. table::
    :widths: auto
    :class: full

    +-------------------------------------------------+------------------------------------------------------------------------------------------+
    | :cpp:func:`imconcretize(f) <mln::imconcretize>` | Creates a new writable image with the *geometry* of `f`.                                 |
    +-------------------------------------------------+------------------------------------------------------------------------------------------+
    | :cpp:func:`imchvalue(f) <mln::imchvalue>`       | Creates a new writable image with the *geomerty* of `f` able to store values of type `U` |
    +-------------------------------------------------+------------------------------------------------------------------------------------------+
    | :cpp:func:`resize(g,f) <mln::resize>`           | Resize `g` to the *geometry* of `f`                                                      |
    +-------------------------------------------------+------------------------------------------------------------------------------------------+

.. topic:: Fundamental image types

  .. table::
    :class: full
    :widths: auto

    +--------------+-------------------------+
    | `image2d<T>` | Buffer-encoded 2D image |
    +--------------+-------------------------+
    | `image3d<T>` | Buffer-encoded 3D image |
    +--------------+-------------------------+
    | `ndimage<T>` | Buffer-encoded nD image |
    +--------------+-------------------------+

.. topic:: Functional image views

  .. table::
    :widths: auto
    :class: full


    +------------------------------------------------------------+--------------------------------------------------------------------------------------------+
    | :cpp:func:`view::transform(ima, f) <mln::view::transform>` | Views the image with a function applied to the values.                                     |
    +------------------------------------------------------------+--------------------------------------------------------------------------------------------+
    | ``view::filter(ima, pred)``                                | Views the image restricted to pixels whose values pass a predicate.                        |
    +------------------------------------------------------------+--------------------------------------------------------------------------------------------+
    | ``view::mask(ima, mask)``                                  | Views the image restricted to pixels in a binary mask.                                     |
    +------------------------------------------------------------+--------------------------------------------------------------------------------------------+
    | ``view::clip(ima, roi)``                                   | Views the image restricted to a sub-region.                                                |
    +------------------------------------------------------------+--------------------------------------------------------------------------------------------+
    | ``view::zip(ima1, ima2, ..., imaN)``                       | Views a list of images as a single image whose values are tuples of the each image values. |
    +------------------------------------------------------------+--------------------------------------------------------------------------------------------+

.. topic:: Common image operators (views)

  .. table::
    :widths: auto
    :class: full

    +--------------+-----------------------------------------------------------+
    | Arithmetical | `a + b`, `a - b`, `a * b`, `a / b`, `a % b`               |
    +--------------+-----------------------------------------------------------+
    | Logical      | `a && b`, `a || b`, `! a`                                 |
    +--------------+-----------------------------------------------------------+
    | Comparison   | `a < b`, `a <= b`, `a == b`, `a != b`, `a >= b`, `a > b`  |
    +--------------+-----------------------------------------------------------+
    | Conditional  | ``view::ifelse(a, b, c)``                                 |
    +--------------+-----------------------------------------------------------+


.. topic:: Mathematical image operator (views)

  +-------+-------+
  | FIXME | FIXME |
  +-------+-------+

.. topic:: Other image operators (views)

  .. table::
    :widths: auto
    :class: full
    
    +---------------------------+---------------------------------------------------+
    | ``view::cast<V>(ima)``    | Views the image with the values casted into ``V`` |
    +---------------------------+---------------------------------------------------+
    | ``view::channel(ima, k)`` | Views the k-th channel of the image               |
    +---------------------------+---------------------------------------------------+
    | ``view::red(ima)``        |                                                   |
    | ``view::green(ima)``      |                                                   |
    | ``view::blue(ima)``       | Views the red, green or blue channel of the image |
    +---------------------------+---------------------------------------------------+


.. raw:: html

         <h2>Ranges</h2>


:doc:`core/ranges` extensions for ranges with new concepts and helper functions.


.. topic:: Concepts

   * :ref:`Segmented Range <concept-ranges-SegmentedRange>`
   * :ref:`Reversible Range <concept-ranges-ReversibleRange>`


.. topic:: Utilities

   * :cpp:func:`mln::ranges::rows`
   * :c:macro:`mln_foreach`

.. topic:: Views

   * :doc:`core/ranges/reverse`
   * :doc:`core/ranges/transform`
   * :doc:`core/ranges/filter`
   * :doc:`core/ranges/zip`
   * :doc:`core/ranges/zip_with`




.. raw:: html

         <h2>Neighborhoods and Structural Elements</h2>


:doc:`core/neighborhood`  are the fundamental tools for local image processing.

.. topic:: Concepts

   * :cpp:concept:`mln::StructuringElement`

.. topic::  Predefined Neighborhood

  * :doc:`core/neighborhood/c4`
  * :doc:`core/neighborhood/c8`

.. topic::  Predefined Structuring Elements

  * :doc:`core/se/disc`
  * :doc:`core/se/rectangle`
  * :doc:`core/se/periodic_lines`





Image primitives
****************

.. toctree::
    :hidden:

    core/algorithm/accumulate    
    core/algorithm/all_any_none_of
    core/algorithm/clone
    core/algorithm/copy
    core/algorithm/count
    core/algorithm/count_if
    core/algorithm/equal
    core/algorithm/fill
    core/algorithm/foreach
    core/algorithm/generate  
    core/algorithm/iota
    core/algorithm/paste
    core/algorithm/sort
    core/algorithm/transform

Fundamental primitives for basic image manipulation. Those are grouped by:

* *Modifying* or *Non-modifying* if they output and write an image
* *Value* or *Pixel* if the localisation (i.e. point/index) is used for processing



.. rubric:: Non-modifying image primitives that operates on values

.. table::
  :class: full
  :widths: auto

  +------------------------------------------------------------+------------------------------------------------------------------------+
  | :doc:`core/algorithm/all_any_none_of`                      | checks if a predicate is true for all, any or none of the image values |
  +------------------------------------------------------------+------------------------------------------------------------------------+
  | :doc:`core/algorithm/foreach`                              | applies a function to the values of an image                           |
  +------------------------------------------------------------+------------------------------------------------------------------------+
  | :doc:`core/algorithm/count` :doc:`core/algorithm/count_if` | returns the number of values satisfying specific criteria              |
  +------------------------------------------------------------+------------------------------------------------------------------------+
  | :doc:`core/algorithm/equal`                                | checks if two images have the same values                              |
  +------------------------------------------------------------+------------------------------------------------------------------------+
  | :doc:`core/algorithm/accumulate` (reduce)                  | sums up values of an image                                             |
  +------------------------------------------------------------+------------------------------------------------------------------------+


.. rubric:: Non-modifying image primitives that operates on pixels


+----------------------------+--------------------------------------------------+
| :doc:`core/algorithm/sort` | returns a container of image points (or indexes) |
+----------------------------+--------------------------------------------------+

.. rubric:: Modifying image primitives that operates on values


.. table::
  :class: full
  :widths: auto

  +---------------------------------+--------------------------------------------------------------+
  | :doc:`core/algorithm/copy`      | copies image values to another image  (without localization) |
  +---------------------------------+--------------------------------------------------------------+
  | :doc:`core/algorithm/fill`      | assigns a value to image values                              |
  +---------------------------------+--------------------------------------------------------------+
  | :doc:`core/algorithm/clone`     | copies an image in a new image                               |
  +---------------------------------+--------------------------------------------------------------+
  | :doc:`core/algorithm/paste`     | copies image pixels to another image (with localization)     |
  +---------------------------------+--------------------------------------------------------------+
  | :doc:`core/algorithm/transform` | applies a function to the values of an image                 |
  +---------------------------------+--------------------------------------------------------------+
  | :doc:`core/algorithm/generate`  | generate the values of an image by a function                |
  +---------------------------------+--------------------------------------------------------------+
  | :doc:`core/algorithm/iota`      | generate the values of an image with increasing value        |
  +---------------------------------+--------------------------------------------------------------+

.. rubric:: Modifying image primitives that operates on pixels

+-------------------------------------------+---------------------------------------------------------------------+
| `integrate` (partial sum, inclusive scan) | computes the partial sum of image values (used for integral images) |
+-------------------------------------------+---------------------------------------------------------------------+

.. raw:: html

         <h2>STL</h2>


:doc:`core/stl` are the fundamentals concepts of the C++ standard library that we are building our concept upon. They are our building blocks.


