Core Module
===========




.. toctree::
   :hidden:

   core/values
   core/points
   core/pixels
   core/domains
   core/ranges
   core/neighborhood
   core/algorithms
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


:doc:`core/points` are the fundamentals tools for locating a value inside an images.


.. topic:: Concepts

   * :ref:`Point <concept-points-Point>`

.. topic:: Predefined points

   * :doc:`core/points/point2d`
   * :doc:`core/points/point3d`




.. raw:: html

         <h2>Pixels</h2>


:doc:`core/pixels` are the fundamentals tools when browsing an images to access both the point and the value.


.. topic:: Concepts

   * :ref:`Pixel <concept-pixels-Pixel>`

.. topic:: Predefined points

   * :doc:`core/pixels/ndpixel`




.. raw:: html

         <h2>Domains</h2>


:doc:`core/domains` are the fundamental tools when expressing the valid set of points for an image.


.. topic:: Concepts

   * :ref:`Domain <concept-domains-Domain>`

.. topic:: Predefined domains

   * :doc:`core/domains/box2d`
   * :doc:`core/domains/box3d`




.. raw:: html

         <h2>Ranges</h2>


:doc:`core/ranges` extensions for ranges with new concepts and helper functions.


.. topic:: Concepts

   * :cpp:concept:`Segmented Range <mln::ranges::SegmentedRange>`
   * :cpp:concept:`Reversible Range <mln::ranges::ReversibleRange>`


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




.. raw:: html

         <h2>Algorithms</h2>


Foundamental and core :doc:`algorithms <core/algorithms>` for image manipulation.




.. raw:: html

         <h2>STL</h2>


:doc:`core/stl` are the fundamentals concepts of the C++ standard library that we are building our concept upon. They are our building blocks.
