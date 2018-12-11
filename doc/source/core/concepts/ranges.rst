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

    .. cpp:var:: SegRng rng

    **Valid Expressions**

    - All expression from :cpp:concept:`ForwardRange (stl) <stl::ForwardRange>` are valid.

    - :cpp:expr:`rng.rows()` return-type models :cpp:concept:`ForwardRange (stl) <stl::ForwardRange>`.
    - :cpp:expr:`stl::iter_value_t<stl::iterator_t<stl::iter_value_t<stl::iterator_t<decltype(rng.rows())>>>>` must be the same as :cpp:expr:`stl::iter_value_t<stl::iterator_t<SegRng>>`.

    **Implementation**

    .. literalinclude:: ../../../../pylene/include/mln/core/concept/new/ranges.hpp
       :language: cpp
       :lines: 15-23


ReversibleRange
###############

.. cpp:concept:: template <typename RevRng> ReversibleRange

     #. Refines the :cpp:concept:`ForwardRange (stl) <stl::ForwardRange>` concept.
    #. ``RevRng`` provides the method ``reversed``.

    **Notation**

    .. cpp:var:: RevRng rng

    **Valid Expressions**

    - All expression from :cpp:concept:`ForwardRange (stl) <stl::ForwardRange>` are valid.

    - :cpp:expr:`rng.reversed()` return-type models :cpp:concept:`ForwardRange (stl) <stl::ForwardRange>`.
    - :cpp:expr:`stl::iter_value_t<stl::iterator_t<decltype(rng.reversed())>>>` must be the same as :cpp:expr:`stl::iter_value_t<stl::iterator_t<RevRng>>`.

    **Implementation**

    .. literalinclude:: ../../../../pylene/include/mln/core/concept/new/ranges.hpp
       :language: cpp
       :lines: 27-35
