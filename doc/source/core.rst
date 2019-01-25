Core Module
===========




.. toctree::
   :hidden:

   core/values
   core/points
   core/pixels
   core/domains
   core/indexes
   core/ranges
   core/images
   core/neighborhood
   core/stl




.. raw:: html

         <h2>Values</h2>


:doc:`core/values` are the fundamentals tools to model values held by images.


.. topic:: Concepts

   * :ref:`Value <concept-values-Value>`
   * :ref:`ComparableValue <concept-values-ComparableValue>`
   * :ref:`OrderedValue <concept-values-OrderedValue>`

.. topic:: Predefined values

   * :doc:`core/values/rgba`

.. topic:: Utilities

   TODO channel utilities
..   * :cpp:func:`is_multichannel`
..   * :cpp:func:`dynamic_channel_get`
..   * :cpp:func:`dynamic_channel_size`
..   * :cpp:func:`static_channel_get`
..   * :cpp:func:`static_channel_size`



.. raw:: html

         <h2>Points</h2>


:doc:`core/points` are the fundamentals tools to locate a value inside an images.


.. topic:: Concepts

   * :ref:`Point <concept-points-Point>`




.. raw:: html

         <h2>Pixels</h2>


:doc:`core/pixels` are the fundamentals tools when browsing an images to access both the point and the value.


.. topic:: Concepts

   * :ref:`Pixel <concept-pixels-Pixel>`




.. raw:: html

         <h2>Domains</h2>


:doc:`core/domains` are the fundamental tools when expressing the valid set of points for an image.


.. topic:: Concepts

   * :ref:`Domain <concept-domains-Domain>`




.. raw:: html

         <h2>Indexes</h2>


:doc:`core/indexes` are the lowest level tools to locate a value inside an images.


.. topic:: Concepts

   * :ref:`Index <concept-indexes-Index>`




.. raw:: html

         <h2>Images</h2>


:doc:`core/images` represent an image and all its properties.


.. topic:: Concepts

   * :ref:`Image <concept-images-Image>`
   * :ref:`IndexableImage <concept-images-IndexableImage>`
   * :ref:`AccessibleImage <concept-images-AccessibleImage>`
   * :ref:`ReversibleImage <concept-images-ReversibleImage>`
   * :ref:`RandomAccessImage <concept-images-RandomAccessImage>`
   * :ref:`ExtendedImage <concept-images-ExtendedImage>`
   * :ref:`ChValueImage <concept-images-ChValueImage>`
   * :ref:`RawImage <concept-images-RawImage>`
   * :ref:`WritableImage <concept-images-WritableImage>`
   * :ref:`ViewImage <concept-images-ViewImage>`
   * :ref:`ConcreteImage <concept-images-ConcreteImage>`


.. topic:: Utilities

   TODO

.. topic:: Images

   * :doc:`core/images/ndimage`




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

    core/algorithm/fill
    core/algorithm/copy
    core/algorithm/paste
    core/algorithm/transform
    core/algorithm/iota
    core/algorithm/accumulate    
    core/algorithm/generate  

Fundamental primitives for basic image manipulation. Those are grouped by:

* *Modifying* or *Non-modifying* if they output and write an image
* *Value* or *Pixel* if the localisation (i.e. point/index) is used for processing



.. rubric:: Non-modifying image primitives that operates on values


+-------------------------------------------+------------------------------------------------------------------------+
| :doc:`core/algorithm/all_any_none_of`     | checks if a predicate is true for all, any or none of the image values |
+-------------------------------------------+------------------------------------------------------------------------+
| :doc:`core/algorithm/foreach`             | applies a function to the values of an image                           |
+-------------------------------------------+------------------------------------------------------------------------+
| `count` `count_if`                        | [TBI] returns the number of values satisfying specific criteria        |
+-------------------------------------------+------------------------------------------------------------------------+
| `equal`                                   | [TBI] checks if two images have the same values                        |
+-------------------------------------------+------------------------------------------------------------------------+
| :doc:`core/algorithm/accumulate` (reduce) | sums up values of an image                                             |
+-------------------------------------------+------------------------------------------------------------------------+


.. rubric:: Non-modifying image primitives that operates on pixels


+----------------------------+--------------------------------------------------+
| :doc:`core/algorithm/sort` | returns a container of image points (or indexes) |
+----------------------------+--------------------------------------------------+

.. rubric:: Modifying image primitives that operates on values


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


