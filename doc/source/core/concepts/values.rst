===============
Values (detail)
===============


.. contents::
   :local:



.. cpp:namespace:: mln::concepts


Value
#####

.. cpp:concept:: template <typename Val> Value

    #. Refines the :cpp:concept:`Semiregular (stl) <stl::Semiregular>` concept.
    #. Limits operations `numeric_limits<Val>` are defined.

    **Notation**

    .. cpp:var:: Val val
    .. cpp:var:: Val val_cpy
    .. cpp:var:: const Val cval

    **Valid Expressions**

    - :cpp:expr:`val()` returns an instance of :cpp:expr:`Val`.
    - :cpp:expr:`cval()` returns an instance of :cpp:expr:`const Val`.

    - :cpp:expr:`val_cpy(val)` returns an instance of :cpp:expr:`Val`.
    - :cpp:expr:`val_cpy(cval)` returns an instance of :cpp:expr:`Val`.
    - :cpp:expr:`val_cpy(move(val))` returns an instance of :cpp:expr:`Val`.

    - :cpp:expr:`val_cpy = val` returns an instance of :cpp:expr:`Val&`.
    - :cpp:expr:`val_cpy = cval` returns an instance of :cpp:expr:`Val&`.
    - :cpp:expr:`val_cpy = move(val)` returns an instance of :cpp:expr:`Val&`.

    - :cpp:expr:`numeric_limits<Val>` is valid. See the `reference documentation <https://en.cppreference.com/w/cpp/types/numeric_limits>`_ for details.

    **Implementation**

    .. literalinclude:: ../../../../pylene/include/mln/core/concept/new/values.hpp
       :language: cpp
       :lines: 12-13


ComparableValue
###############

.. cpp:concept:: template <typename CompVal> ComparableValue

    #. Refines the :cpp:concept:`Value <Value>` concept.
    #. Refines the :cpp:concept:`Regular (stl) <stl::Regular>` concept.

    **Notation**

    .. cpp:var:: const CompVal lhs
    .. cpp:var:: const CompVal rhs

    **Valid Expressions**

    - :cpp:expr:`lhs == rhs` return-type models :cpp:concept:`Boolean (stl) <stl::Boolean>`.
    - :cpp:expr:`lhs != rhs` return-type models :cpp:concept:`Boolean (stl) <stl::Boolean>`.
    - :cpp:expr:`rhs == lhs` return-type models :cpp:concept:`Boolean (stl) <stl::Boolean>`.
    - :cpp:expr:`rhs != lhs` return-type models :cpp:concept:`Boolean (stl) <stl::Boolean>`.

    **Implementation**

    .. literalinclude:: ../../../../pylene/include/mln/core/concept/new/values.hpp
       :language: cpp
       :lines: 17-20


OrderedValue
############

.. cpp:concept:: template <typename OrdVal> OrderedValue

    #. Refines the :cpp:concept:`ComparableValue <ComparableValue>` concept.
    #. Refines the :cpp:concept:`StrictTotallyOrdered (stl) <stl::StrictTotallyOrdered>` concept.

    **Notation**

    .. cpp:var:: const OrdVal lhs
    .. cpp:var:: const OrdVal rhs

    **Valid Expressions**

    - :cpp:expr:`lhs < rhs` return-type models :cpp:concept:`Boolean (stl) <stl::Boolean>`.
    - :cpp:expr:`lhs > rhs` return-type models :cpp:concept:`Boolean (stl) <stl::Boolean>`.
    - :cpp:expr:`lhs <= rhs` return-type models :cpp:concept:`Boolean (stl) <stl::Boolean>`.
    - :cpp:expr:`lhs >= rhs` return-type models :cpp:concept:`Boolean (stl) <stl::Boolean>`.

    **Implementation**

    .. literalinclude:: ../../../../pylene/include/mln/core/concept/new/values.hpp
       :language: cpp
       :lines: 24-27

