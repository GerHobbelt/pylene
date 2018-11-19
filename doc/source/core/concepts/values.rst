===============
Values (detail)
===============


.. contents::
   :local:



.. cpp:namespace:: mln::core::concepts


Value
#####

.. cpp:concept:: template <typename Val> Value

    #. Refines the :cpp:concept:`SemiregularValue <SemiregularValue>` concept.
    #. Limits operations `numeric_limits<Val>` are defined.

    **Notation**

    .. cpp:val: const Val v
    .. cpp:val: const Val lhs
    .. cpp:val: const Val rhs

    **Valid Expressions**

    - :cpp:expr:`numeric_limits<Val>` is valid. See the `reference documentation <https://en.cppreference.com/w/cpp/types/numeric_limits>`_ for details.

    **Implementation**

    .. literalinclude:: ../../../../pylene/include/mln/core/concept/new/values.hpp
       :language: cpp
       :lines: 16-17


SemiregularValue
################

.. cpp:concept:: template <typename SemiregVal> SemiregularValue

    #. Refines the :cpp:concept:`Semiregular (stl) <stl::Semiregular>` concept.

    **Notation**

    .. cpp:var::    SemiregVal val
    .. cpp:var::    SemiregVal val_cpy
    .. cpp:var::    const SemiregVal cval

    **Valid Expressions**

    - :cpp:expr:`val()` returns an instance of :cpp:expr:`SemiregVal`.
    - :cpp:expr:`cval()` returns an instance of :cpp:expr:`const SemiregVal`.

    - :cpp:expr:`val_cpy(val)` returns an instance of :cpp:expr:`SemiregVal`.
    - :cpp:expr:`val_cpy(cval)` returns an instance of :cpp:expr:`SemiregVal`.
    - :cpp:expr:`val_cpy(move(val))` returns an instance of :cpp:expr:`SemiregVal`.

    - :cpp:expr:`val_cpy = val` returns an instance of :cpp:expr:`SemiregVal&`.
    - :cpp:expr:`val_cpy = cval` returns an instance of :cpp:expr:`SemiregVal&`.
    - :cpp:expr:`val_cpy = move(val)` returns an instance of :cpp:expr:`SemiregVal&`.

    **Implementation**

    .. literalinclude:: ../../../../pylene/include/mln/core/concept/new/values.hpp
       :language: cpp
       :lines: 11-12


RegularValue
############

.. cpp:concept:: template <typename RegVal> RegularValue

    #. Refines the :cpp:concept:`SemiregularValue <SemiregularValue>` concept.
    #. Refines the :cpp:concept:`Regular (stl) <stl::Regular>` concept.

    **Notation**

    .. cpp:var::    const RegVal lhs
    .. cpp:var::    const RegVal rhs

    **Valid Expressions**

    - :cpp:expr:`lhs == rhs` return-type models :cpp:concept:`Boolean (stl) <stl::Boolean>`.
    - :cpp:expr:`lhs != rhs` return-type models :cpp:concept:`Boolean (stl) <stl::Boolean>`.
    - :cpp:expr:`rhs == lhs` return-type models :cpp:concept:`Boolean (stl) <stl::Boolean>`.
    - :cpp:expr:`rhs != lhs` return-type models :cpp:concept:`Boolean (stl) <stl::Boolean>`.

    **Implementation**

    .. literalinclude:: ../../../../pylene/include/mln/core/concept/new/values.hpp
       :language: cpp
       :lines: 21-24


StrictTotallyOrderedRegularValue
################################

.. cpp:concept:: template <typename STORegVal> StrictTotallyOrderedRegularValue

    #. Refines the :cpp:concept:`RegularValue <RegularValue>` concept.
    #. Refines the :cpp:concept:`StrictTotallyOrdered (stl) <stl::StrictTotallyOrdered>` concept.

    **Notation**

    .. cpp:var::    const STORegVal lhs
    .. cpp:var::    const STORegVal rhs

    **Valid Expressions**

    - :cpp:expr:`lhs < rhs` return-type models :cpp:concept:`Boolean (stl) <stl::Boolean>`.
    - :cpp:expr:`lhs > rhs` return-type models :cpp:concept:`Boolean (stl) <stl::Boolean>`.
    - :cpp:expr:`lhs <= rhs` return-type models :cpp:concept:`Boolean (stl) <stl::Boolean>`.
    - :cpp:expr:`lhs >= rhs` return-type models :cpp:concept:`Boolean (stl) <stl::Boolean>`.

    **Implementation**

    .. literalinclude:: ../../../../pylene/include/mln/core/concept/new/values.hpp
       :language: cpp
       :lines: 28-31

