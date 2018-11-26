===============
Ranges (detail)
===============


.. contents::
   :local:



.. cpp:namespace:: mln::concepts


SegmentedRange
##############

.. cpp:concept:: template <typename SegRng> SegmentedRange

    #. Refines the :cpp:concept:`ForwardRange (stl) <stl::ForwardRange>` concept.
    #. ``SegRng`` must derive from ``multidimensional_range_base``.
    #. ``SegRng`` provides the method ``rows``.

    **Notation**

    .. cpp:var:: const SegRng crng

    **Valid Expressions**

    - All expression from :cpp:concept:`ForwardRange (stl) <stl::ForwardRange>` are valid.

    - :cpp:expr:`std::is_base_of_v<mln::ranges::multidimensional_range_base, SegRng>` returns ``true``.

    - :cpp:expr:`crng.rows()` return-type models :cpp:concept:`ForwardRange (stl) <stl::ForwardRange>`.

    **Implementation**d

    .. literalinclude:: ../../../../pylene/include/mln/core/concept/new/ranges.hpp
       :language: cpp
       :lines: 15-21


ReversibleRange
###############

.. cpp:concept:: template <typename RevRng> ReversibleRange

     #. Refines the :cpp:concept:`ForwardRange (stl) <stl::ForwardRange>` concept.
    #. ``RevRng`` provides the method ``reversed``.

    **Notation**

    .. cpp:var:: const RevRng crng

    **Valid Expressions**

    - All expression from :cpp:concept:`ForwardRange (stl) <stl::ForwardRange>` are valid.

    - :cpp:expr:`crng.reversed()` return-type models :cpp:concept:`ForwardRange (stl) <stl::ForwardRange>`.

    **Implementation**d

    .. literalinclude:: ../../../../pylene/include/mln/core/concept/new/ranges.hpp
       :language: cpp
       :lines: 25-30
