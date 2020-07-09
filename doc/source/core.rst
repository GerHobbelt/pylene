Core Module
===========

.. cpp:namespace:: mln


.. toctree::
   :hidden:

   core/core_types
   core/images
   core/ranges
   core/neighborhood
   core/stl
   core/functional


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

.. topic:: Fundamental types

  .. table::
    :class: full
    :widths: auto

    +----------------------------------------------+-------------------------------------------------------------+
    | :cpp:class:`ndpoint` :cpp:class:`ndpointref` | Generic :doc:`point <core/point>` that hold *n* coordinates |
    +----------------------------------------------+-------------------------------------------------------------+
    | :cpp:class:`ndbox` :cpp:class:`ndboxref`     | Generic :doc:`box <core/box>` in *n* dimension              |
    +----------------------------------------------+-------------------------------------------------------------+






.. topic:: Fundamental image types

  .. table::
    :class: full
    :widths: auto

    +-----------------------------------------+-------------------------+
    |         :cpp:any:`image2d`              | Buffer-encoded 2D image |
    +-----------------------------------------+-------------------------+
    |         :cpp:any:`image3d`              | Buffer-encoded 3D image |
    +-----------------------------------------+-------------------------+
    |         :cpp:any:`ndimage`              | Buffer-encoded nD image |
    +-----------------------------------------+-------------------------+


.. topic:: Functional image views

  .. table::
    :widths: auto
    :class: full

    +---------------------------------------------------------------+--------------------------------------------------------------------------------------------+
    | :cpp:func:`view::transform(ima, f) <mln::view::transform>`    | Views the image with a function applied to the values.                                     |
    +---------------------------------------------------------------+--------------------------------------------------------------------------------------------+
    | :cpp:func:`view::filter(ima, pred) <mln::view::filter>`       | Views the image restricted to pixels whose values pass a predicate.                        |
    +---------------------------------------------------------------+--------------------------------------------------------------------------------------------+
    | :cpp:func:`view::mask(ima, mask) <mln::view::mask>`           | Views the image restricted to pixels in a binary mask.                                     |
    +---------------------------------------------------------------+--------------------------------------------------------------------------------------------+
    | :cpp:func:`view::clip(ima, roi) <mln::view::clip>`            | Views the image restricted to a sub-region.                                                |
    +---------------------------------------------------------------+--------------------------------------------------------------------------------------------+
    | :cpp:func:`view::zip(ima1, ima2, ..., imaN) <mln::view::zip>` | Views a list of images as a single image whose values are tuples of the each image values. |
    +---------------------------------------------------------------+--------------------------------------------------------------------------------------------+


.. topic:: Common image operators (views)

  .. table::
    :widths: auto
    :class: full

    +--------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    | Arithmetical | :cpp:func:`a + b <mln::view::ops::operator+>`, :cpp:func:`a - b <mln::view::ops::operator->`, :cpp:func:`a * b <mln::view::ops::operator*>`, :cpp:func:`a / b <mln::view::ops::operator/>`, :cpp:func:`a % b <mln::view::ops::operator%>`                                                                                                                    |
    +--------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    | Logical      | :cpp:func:`a && b <mln::view::ops::operator&&>`, :cpp:func:`a || b <mln::view::ops::operator||>`, :cpp:func:`\!a <mln::view::ops::operator!>`                                                                                                                                                                                                                |
    +--------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    | Comparison   | :cpp:func:`a \< b <mln::view::ops::operator<>`, :cpp:func:`a \<= b <mln::view::ops::operator<=>`, :cpp:func:`a == b <mln::view::ops::operator==>`, :cpp:func:`a != b <mln::view::ops::operator!=>`, :cpp:func:`a >= b <mln::view::ops::operator>=>`, :cpp:func:`a > b <mln::view::ops::operator>>`, :cpp:func:`equalFP(a, b, eps) <mln::view::ops::equalFP>` |
    +--------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    | Conditional  | :cpp:func:`view::ifelse(a, b, c) <mln::view::ifelse>`                                                                                                                                                                                                                                                                                                        |
    +--------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+


.. topic:: Mathematical image operator (views)

  .. table::
    :widths: auto
    :class: full

    +----------------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    | Component-wise | :cpp:func:`abs(a) <mln::view::maths::abs>`, :cpp:func:`sqr(a) <mln::view::maths::sqr>`, :cpp:func:`pow(a, p) <mln::view::maths::pow>`, :cpp:func:`cbrt(a) <mln::view::maths::cbrt>`, :cpp:func:`sqrt(a) <mln::view::maths::sqrt>`                                                                                                                                     |
    +----------------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    | Reduction      | :cpp:func:`sum(a) <mln::view::maths::sum>`, :cpp:func:`prod(a) <mln::view::maths::prod>`, :cpp:func:`min(a) <mln::view::maths::min>`, :cpp:func:`max(a) <mln::view::maths::max>`, :cpp:func:`dot(a, b) <mln::view::maths::dot>`, :cpp:func:`cross(a, b) <mln::view::maths::cross>`                                                                                    |
    +----------------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    | Norms          | :cpp:func:`l0norm(a) <mln::view::maths::l0norm>`, :cpp:func:`l1norm(a) <mln::view::maths::l1norm>`, :cpp:func:`l2norm(a) <mln::view::maths::l2norm>`, :cpp:func:`l2norm_sqr(a) <mln::view::maths::l2norm_sqr>`, :cpp:func:`linfnorm(a) <mln::view::maths::linfnorm>`, :cpp:func:`lpnorm\<p>(a) <template <unsigned V> mln::view::maths::lpnorm<V>>`                   |
    +----------------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    | Distances      | :cpp:func:`l0dist(a, b) <mln::view::maths::l0dist>`, :cpp:func:`l1dist(a, b) <mln::view::maths::l1dist>`, :cpp:func:`l2dist(a, b) <mln::view::maths::l2dist>`, :cpp:func:`l2dist_sqr(a, b) <mln::view::maths::l2dist_sqr>`, :cpp:func:`linfdist(a, b) <mln::view::maths::linfdist>`, :cpp:func:`lpdist\<p>(a, b) <template <unsigned V> mln::view::maths::lpdist<V>>` |
    +----------------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+


.. topic:: Other image operators (views)

  .. table::
    :widths: auto
    :class: full
    
    +----------------------------------------------------------------------------+---------------------------------------------------+
    | :cpp:func:`view::cast\<T>(ima) <template <typename T> mln::view::cast<T>>` | Views the image with the values casted into ``T`` |
    +----------------------------------------------------------------------------+---------------------------------------------------+
    | :cpp:func:`view::channel(ima, k) <mln::view::channel>`                     | Views the k-th channel of the image               |
    +----------------------------------------------------------------------------+---------------------------------------------------+
    | :cpp:func:`view::red(ima) <mln::view::red>`                                |                                                   |
    | :cpp:func:`view::green(ima) <mln::view::green>`                            | Views the red, green or blue channel of the image |
    | :cpp:func:`view::blue(ima) <mln::view::blue>`                              |                                                   |
    +----------------------------------------------------------------------------+---------------------------------------------------+


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
  * :doc:`core/neighborhood/c6`
  * :doc:`core/neighborhood/c26`

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
