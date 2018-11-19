Core Module
===========




.. toctree::
   :hidden:

   core/values
   core/points
   core/pixels
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


.. topic:: Concepts

   * Core language
 
      * :cpp:concept:`Same <mln::core::concepts::stl::Same>`
      * :cpp:concept:`DerivedFrom <mln::core::concepts::stl::DerivedFrom>`
      * :cpp:concept:`ConvertibleTo <mln::core::concepts::stl::ConvertibleTo>`
      * :cpp:concept:`CommonReference <mln::core::concepts::stl::CommonReference>`
      * :cpp:concept:`Common <mln::core::concepts::stl::Common>`
      * :cpp:concept:`Integral <mln::core::concepts::stl::Integral>`
      * :cpp:concept:`SignedIntegral <mln::core::concepts::stl::SignedIntegral>`
      * :cpp:concept:`UnsignedIntegral <mln::core::concepts::stl::UnsignedIntegral>`
      * :cpp:concept:`Assignable <mln::core::concepts::stl::Assignable>`
      * :cpp:concept:`Swappable <mln::core::concepts::stl::Swappable>`
      * :cpp:concept:`SwappableWith <mln::core::concepts::stl::SwappableWith>`
      * :cpp:concept:`Destructible <mln::core::concepts::stl::Destructible>`
      * :cpp:concept:`Constructible <mln::core::concepts::stl::Constructible>`
      * :cpp:concept:`DefaultConstructible <mln::core::concepts::stl::DefaultConstructible>`
      * :cpp:concept:`MoveConstructible <mln::core::concepts::stl::MoveConstructible>`
      * :cpp:concept:`CopyConstructible <mln::core::concepts::stl::CopyConstructible>`
 
   * Object

      * :cpp:concept:`Movable <mln::core::concepts::stl::Movable>`
      * :cpp:concept:`Copyable <mln::core::concepts::stl::Copyable>`
      * :cpp:concept:`Semiregular <mln::core::concepts::stl::Semiregular>`
      * :cpp:concept:`Regular <mln::core::concepts::stl::Regular>`
 
   * Comparison
      
      * :cpp:concept:`Boolean <mln::core::concepts::stl::Boolean>`
      * :cpp:concept:`\__WeaklyEqualityComparableWith <mln::core::concepts::stl::__WeaklyEqualityComparableWith>`
      * :cpp:concept:`EqualityComparable <mln::core::concepts::stl::EqualityComparable>`
      * :cpp:concept:`EqualityComparableWith <mln::core::concepts::stl::EqualityComparableWith>`
      * :cpp:concept:`StrictTotallyOrdered <mln::core::concepts::stl::StrictTotallyOrdered>`
      * :cpp:concept:`StrictTotallyOrderedWith <mln::core::concepts::stl::StrictTotallyOrderedWith>`
 
   * Callable
      
      * :cpp:concept:`Invocable <mln::core::concepts::stl::Invocable>`
      * :cpp:concept:`InvocableR <mln::core::concepts::stl::InvocableR>`
      * :cpp:concept:`RegularInvocable <mln::core::concepts::stl::RegularInvocable>`
      * :cpp:concept:`RegularInvocableR <mln::core::concepts::stl::RegularInvocableR>`
      * :cpp:concept:`Predicate <mln::core::concepts::stl::Predicate>`
      * :cpp:concept:`Relation <mln::core::concepts::stl::Relation>`
      * :cpp:concept:`StrictWeakOrder <mln::core::concepts::stl::StrictWeakOrder>`
 
   * Random number generator

      * :cpp:concept:`UniformRandomNumberGenerator <mln::core::concepts::stl::UniformRandomNumberGenerator>`
 
   * Iterator

      * :cpp:concept:`Readable <mln::core::concepts::stl::Readable>`
      * :cpp:concept:`Writable <mln::core::concepts::stl::Writable>`
      * :cpp:concept:`WeaklyIncrementable <mln::core::concepts::stl::WeaklyIncrementable>`
      * :cpp:concept:`Incrementable <mln::core::concepts::stl::Incrementable>`
      * :cpp:concept:`Iterator <mln::core::concepts::stl::Iterator>`
      * :cpp:concept:`Sentinel <mln::core::concepts::stl::Sentinel>`
      * :cpp:concept:`SizedSentinel <mln::core::concepts::stl::SizedSentinel>`
      * :cpp:concept:`InputIterator <mln::core::concepts::stl::InputIterator>`
      * :cpp:concept:`OutputIterator <mln::core::concepts::stl::OutputIterator>`
      * :cpp:concept:`ForwardIterator <mln::core::concepts::stl::ForwardIterator>`
      * :cpp:concept:`BidirectionalIterator <mln::core::concepts::stl::BidirectionalIterator>`
      * :cpp:concept:`RandomAccessIterator <mln::core::concepts::stl::RandomAccessIterator>`
 
   * Indirect callable

      * :cpp:concept:`IndirectUnaryInvocable <mln::core::concepts::stl::IndirectUnaryInvocable>`
      * :cpp:concept:`IndirectRegularUnaryInvocable <mln::core::concepts::stl::IndirectRegularUnaryInvocable>`
      * :cpp:concept:`IndirectUnaryPredicate <mln::core::concepts::stl::IndirectUnaryPredicate>`
      * :cpp:concept:`IndirectRelation <mln::core::concepts::stl::IndirectRelation>`
      * :cpp:concept:`IndirectStrictWeakOrder <mln::core::concepts::stl::IndirectStrictWeakOrder>`
 
   * Common algorithm requirements

      * :cpp:concept:`IndirectlyMovable <mln::core::concepts::stl::IndirectlyMovable>`
      * :cpp:concept:`IndirectlyMovableStorable <mln::core::concepts::stl::IndirectlyMovableStorable>`
      * :cpp:concept:`IndirectlyCopyable <mln::core::concepts::stl::IndirectlyCopyable>`
      * :cpp:concept:`IndirectlyCopyableStorable <mln::core::concepts::stl::IndirectlyCopyableStorable>`
      * :cpp:concept:`IndirectlySwappable <mln::core::concepts::stl::IndirectlySwappable>`
      * :cpp:concept:`IndirectlyComparable <mln::core::concepts::stl::IndirectlyComparable>`
      * :cpp:concept:`Permutable <mln::core::concepts::stl::Permutable>`
      * :cpp:concept:`Mergeable <mln::core::concepts::stl::Mergeable>`
      * :cpp:concept:`Sortable <mln::core::concepts::stl::Sortable>`
 
   * Range
      
      * :cpp:concept:`Range <mln::core::concepts::stl::Range>`
      * :cpp:concept:`SizedRange <mln::core::concepts::stl::SizedRange>`
      * :cpp:concept:`View <mln::core::concepts::stl::View>`
      * :cpp:concept:`BoundedRange <mln::core::concepts::stl::BoundedRange>`
      * :cpp:concept:`InputRange <mln::core::concepts::stl::InputRange>`
      * :cpp:concept:`OutputRange <mln::core::concepts::stl::OutputRange>`
      * :cpp:concept:`ForwardRange <mln::core::concepts::stl::ForwardRange>`
      * :cpp:concept:`BidirectionalRange <mln::core::concepts::stl::BidirectionalRange>`
      * :cpp:concept:`RandomAccessRange <mln::core::concepts::stl::RandomAccessRange>`
