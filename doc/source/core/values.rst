======
Values
======


Values to be stored in an images.



.. contents::
   :local:


Concepts
--------


.. cpp:namespace:: mln::core::concepts


.. _concept-values-Value:

Value
#####

Let `Val` be a type that models :cpp:concept:`Value (details) <Value>`.

Then `Val` also models :ref:`SemiregularValue <concept-values-SemiregularValue>`

.. _concept-values-Value-expressions:
.. rubric:: Expressions

Let `Val` inherit all valid expressions defined for :ref:`SemiregularValue <concept-values-SemiregularValue-expressions>`.

Then ``numeric_limits<Val>`` must be specialized and well-behaved.
See the `reference documentation <https://en.cppreference.com/w/cpp/types/numeric_limits>`_ for details about the specialization.



.. _concept-values-SemiregularValue:

SemiregularValue
################

Let `SemiregVal` be a type that models :cpp:concept:`SemiregularValue (details) <SemiregularValue>`.

Then `SemiregVal` also models : :cpp:concept:`Semiregular (stl) <stl::Semiregular>`.

.. _concept-values-SemiregularValue-expressions:
.. rubric:: Expressions

Let : 
    - ``val`` be an instance of `SemiregVal`.
    - ``val_cpy`` be an instance of `SemiregVal`.
    - ``cval`` be an instance of `const SemiregVal`.

Then we have the following valid expressions :

+-------------------------+--------------------+---------------+----------------+---------------------------------------------------+
|       Expression        |    Return type     | Pre-condition | Post-condition |                    Description                    |
+=========================+====================+===============+================+===================================================+
| ``val()``               | `SemiregVal`       | none          | none           | Constructs a value ``val`` of type `SemiregVal`   |
+-------------------------+--------------------+---------------+----------------+---------------------------------------------------+
| ``cval()``              | `const SemiregVal` | none          | none           | Constructs a value ``cval`` of type `SemiregVal`  |
+-------------------------+--------------------+---------------+----------------+---------------------------------------------------+
| ``val_cpy(val)``        | `SemiregVal`       | none          | none           | Copy-constructs a value ``val_cpy`` from ``val``  |
+-------------------------+--------------------+---------------+----------------+---------------------------------------------------+
| ``val_cpy(cval)``       | `SemiregVal`       | none          | none           | Copy-constructs a value ``val_cpy`` from ``cval`` |
+-------------------------+--------------------+---------------+----------------+---------------------------------------------------+
| ``val_cpy(move(val))``  | `SemiregVal`       | none          | none           | Move-constructs a value ``val_cpy`` from ``val``  |
+-------------------------+--------------------+---------------+----------------+---------------------------------------------------+
| ``val_cpy = val``       | `SemiregVal&`      | none          | none           | Assign ``val`` to ``val_cpy``                     |
+-------------------------+--------------------+---------------+----------------+---------------------------------------------------+
| ``val_cpy = cval``      | `SemiregVal&`      | none          | none           | Assign ``cval`` to ``val_cpy``                    |
+-------------------------+--------------------+---------------+----------------+---------------------------------------------------+
| ``val_cpy = move(val)`` | `SemiregVal&`      | none          | none           | Move-assign ``val`` to ``val_cpy``                |
+-------------------------+--------------------+---------------+----------------+---------------------------------------------------+



.. _concept-values-RegularValue:

RegularValue
############

Let `RegVal` be a type that models :cpp:concept:`RegularValue (details) <RegularValue>`.

Let `RegVal` also models :ref:`SemiregularValue <concept-values-SemiregularValue>` and :cpp:concept:`Regular (stl) <stl::Regular>`.

.. _concept-values-RegularValue-expressions:
.. rubric:: Expressions

Let `RegVal` inherit all valid expressions defined for :ref:`SemiregularValue <concept-values-SemiregularValue-expressions>`.

Let : 
    - ``lhs`` be an instance of `const RegVal`.
    - ``rhs`` be an instance of `const RegVal`.

Then we have the following valid expressions :

+----------------+-------------+---------------+----------------+-----------------------------------------+
|   Expression   | Return type | Pre-condition | Post-condition |               Description               |
+================+=============+===============+================+=========================================+
| ``lhs == rhs`` | `bool`      | none          | none           | Compare wether two values are equal     |
+----------------+-------------+---------------+----------------+-----------------------------------------+
| ``lhs != rhs`` | `bool`      | none          | none           | Compare wether two values are different |
+----------------+-------------+---------------+----------------+-----------------------------------------+
| ``rhs == rhs`` | `bool`      | none          | none           | Compare wether two values are equal     |
+----------------+-------------+---------------+----------------+-----------------------------------------+
| ``rhs != rhs`` | `bool`      | none          | none           | Compare wether two values are different |
+----------------+-------------+---------------+----------------+-----------------------------------------+



.. _concept-values-StrictTotallyOrderedRegularValue:

StrictTotallyOrderedRegularValue
################################

Let `STORegVal` be a type that models :cpp:concept:`StrictTotallyOrderedRegularValue (details) <StrictTotallyOrderedRegularValue>`.

Let `STORegVal` also models :ref:`RegularValue <concept-values-RegularValue>` and :cpp:concept:`StrictTotallyOrdered (stl) <stl::StrictTotallyOrdered>`.

.. _concept-values-StrictTotallyOrderedRegularValue-expressions:
.. rubric:: Expressions

Let `STORegVal` inherit all valid expressions defined for :ref:`RegularValue <concept-values-RegularValue-expressions>`.

Let : 
    - ``lhs`` be an instance of `const STORegVal`.
    - ``rhs`` be an instance of `const STORegVal`.

Then we have the following valid expressions :

+----------------+-------------+---------------+----------------+---------------------------------+
|   Expression   | Return type | Pre-condition | Post-condition |           Description           |
+================+=============+===============+================+=================================+
| ``lhs < rhs``  | `bool`      | none          | none           | Compare order of the two values |
+----------------+-------------+---------------+----------------+---------------------------------+
| ``lhs > rhs``  | `bool`      | none          | none           | Compare order of the two values |
+----------------+-------------+---------------+----------------+---------------------------------+
| ``lhs <= rhs`` | `bool`      | none          | none           | Compare order of the two values |
+----------------+-------------+---------------+----------------+---------------------------------+
| ``lhs >= rhs`` | `bool`      | none          | none           | Compare order of the two values |
+----------------+-------------+---------------+----------------+---------------------------------+




Predefined values
-----------------

.. toctree::

   values/rgba




Concepts detail
---------------

.. toctree::
   
   concepts/values
