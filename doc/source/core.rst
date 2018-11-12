Core Module
===========




.. toctree::
   :hidden:

   core/ranges
   core/neighborhood
   core/algorithms




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





.. raw:: html

         <h2>Neighborhoods and Structural Elements</h2>

:doc:`core/neighborhood`  are the fundamental tools for local image processing.

.. topic:: Concepts

   * :cpp:concept:`mln::StructuringElement`

* Predefined Neighborhood

  * :doc:`core/neighborhood/c4`
  * :doc:`core/neighborhood/c8`

* Predefined Structuring Elements

  * :doc:`core/se/disc`
  * :doc:`core/se/rectangle`
  * :doc:`core/se/periodic_lines`



.. raw:: html

         <h2>Algorithms</h2>


Foundamental and core :doc:`algorithms <core/algorithms>` for image manipulation.
