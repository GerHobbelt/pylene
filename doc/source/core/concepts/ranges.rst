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
    #. ``SegRng`` provides the method ``rows``.

    **Notation**

    .. cpp:var:: const SegRng crng

    **Valid Expressions**

    - All expression from :cpp:concept:`ForwardRange (stl) <stl::ForwardRange>` are valid.

    - :cpp:expr:`crng.rows()` return-type models :cpp:concept:`ForwardRange (stl) <stl::ForwardRange>`.

    **Implementation**d

    .. literalinclude:: ../../../../pylene/include/mln/core/concept/new/ranges.hpp
       :language: cpp
       :lines: 15-23


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
       :lines: 27-35
