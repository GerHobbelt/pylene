=================
Domains (details)
=================


.. contents:: Table of Contents
    :local:



.. cpp:namespace:: mln::concepts


Domain
######

.. cpp:concept:: template<typename D> Domain

    #. Refines the :cpp:concept:`Regular <stl::Regular>` concept.
    #. ``Dom::point_type`` models :cpp:concept:`Point <Point>` concept.
    #. ``Dom::codomain_type`` models :cpp:concept:`Value <Value>` concept, is non-const and is not a reference.
    #. ``Dom`` provides methods `has`, `shape`, `empty`, `is_continuous`, `is_discrete` and `points`.

    **Notation**

    .. cpp:type::   Dom
    .. cpp:type::   point_type = Dom::point_type
    .. cpp:var::    Dom dom
    .. cpp:var::    Dom dom_cpy
    .. cpp:var::    const Dom cdom
    .. cpp:var::    const Dom lhs
    .. cpp:var::    const Dom rhs
    .. cpp:var::    point_type p

    **Valid Expressions**

    - :cpp:expr:`dom()` returns an instance of :cpp:expr:`Dom`.
    - :cpp:expr:`cdom()` returns an instance of :cpp:expr:`const Dom`.

    - :cpp:expr:`dom_cpy(dom)` returns an instance of :cpp:expr:`Dom`.
    - :cpp:expr:`dom_cpy(cdom)` returns an instance of :cpp:expr:`Dom`.
    - :cpp:expr:`dom_cpy(move(dom))` returns an instance of :cpp:expr:`Dom`.

    - :cpp:expr:`dom_cpy = dom` returns an instance of :cpp:expr:`Dom&`.
    - :cpp:expr:`dom_cpy = cdom` returns an instance of :cpp:expr:`Dom&`.
    - :cpp:expr:`dom_cpy = move(dom)` returns an instance of :cpp:expr:`Dom&`.

    - :cpp:expr:`lhs == rhs` return-type models :cpp:concept:`Boolean <stl::Boolean>`.
    - :cpp:expr:`lhs != rhs` return-type models :cpp:concept:`Boolean <stl::Boolean>`.
    - :cpp:expr:`rhs == lhs` return-type models :cpp:concept:`Boolean <stl::Boolean>`.
    - :cpp:expr:`rhs != lhs` return-type models :cpp:concept:`Boolean <stl::Boolean>`.

    - :cpp:expr:`cdom.has(p)` return-type models :cpp:concept:`Boolean <stl::Boolean>`.
    - :cpp:expr:`cdom.empty()` return-type models :cpp:concept:`Boolean <stl::Boolean>`.
    - :cpp:expr:`cdom.is_continuous()` return-type models :cpp:concept:`Boolean <stl::Boolean>`.
    - :cpp:expr:`cdom.is_discrete()` return-type models :cpp:concept:`Boolean <stl::Boolean>`.
    - :cpp:expr:`cdom.points()` return-type models :cpp:concept:`BidirectionalRange <stl::BidirectionalRange>`.

    **Implementation**

    .. literalinclude:: ../../../../pylene/include/mln/core/concept/new/domains.hpp
       :language: cpp
       :lines: 16-33
