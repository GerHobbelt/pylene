================
Points (details)
================


.. contents:: Table of Contents
    :local:



.. cpp:namespace:: mln::concepts


Point
#####

.. cpp:concept:: template <typename Pnt> Point

    #. Refines the :cpp:concept:`Regular (stl) <stl::Regular>` concept.
    #. Refines the :cpp:concept:`StrictTotallyOrdered (stl) <stl::StrictTotallyOrdered>` concept.

    **Notation**

    .. cpp:var::    Pnt pnt
    .. cpp:var::    Pnt pnt_cpy
    .. cpp:var::    const Pnt cpnt
    .. cpp:var::    const Pnt lhs
    .. cpp:var::    const Pnt rhs

    **Valid Expressions**

    - :cpp:expr:`pnt()` returns an instance of :cpp:expr:`Pnt`.
    - :cpp:expr:`cpnt()` returns an instance of :cpp:expr:`const Pnt`.

    - :cpp:expr:`pnt_cpy(pnt)` returns an instance of :cpp:expr:`Pnt`.
    - :cpp:expr:`pnt_cpy(cpnt)` returns an instance of :cpp:expr:`Pnt`.
    - :cpp:expr:`pnt_cpy(move(pnt))` returns an instance of :cpp:expr:`Pnt`.

    - :cpp:expr:`pnt_cpy = pnt` returns an instance of :cpp:expr:`Pnt&`.
    - :cpp:expr:`pnt_cpy = cpnt` returns an instance of :cpp:expr:`Pnt&`.
    - :cpp:expr:`pnt_cpy = move(pnt)` returns an instance of :cpp:expr:`Pnt&`.

    - :cpp:expr:`lhs == rhs` return-type models :cpp:concept:`Boolean (stl) <stl::Boolean>`.
    - :cpp:expr:`lhs != rhs` return-type models :cpp:concept:`Boolean (stl) <stl::Boolean>`.
    - :cpp:expr:`rhs == lhs` return-type models :cpp:concept:`Boolean (stl) <stl::Boolean>`.
    - :cpp:expr:`rhs != lhs` return-type models :cpp:concept:`Boolean (stl) <stl::Boolean>`.

    - :cpp:expr:`lhs < rhs` return-type models :cpp:concept:`Boolean (stl) <stl::Boolean>`.
    - :cpp:expr:`lhs > rhs` return-type models :cpp:concept:`Boolean (stl) <stl::Boolean>`.
    - :cpp:expr:`lhs <= rhs` return-type models :cpp:concept:`Boolean (stl) <stl::Boolean>`.
    - :cpp:expr:`lhs >= rhs` return-type models :cpp:concept:`Boolean (stl) <stl::Boolean>`.

    **Implementation**

    .. literalinclude:: ../../../../pylene/include/mln/core/concepts/point.hpp
       :language: cpp
       :lines: 12-15
