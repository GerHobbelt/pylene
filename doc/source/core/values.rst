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

Then `Val` also models :cpp:concept:`Semiregular (stl) <stl::Semiregular>`

.. _concept-values-Value-expressions:
.. rubric:: Expressions

Let : 
    - ``val`` be an instance of `Val`.
    - ``val_cpy`` be an instance of `Val`.
    - ``cval`` be an instance of `const Val`.

Then ``numeric_limits<Val>`` must be specialized and well-behaved.
See the `reference documentation <https://en.cppreference.com/w/cpp/types/numeric_limits>`_ for details about the specialization.

Then we have the following valid expressions :

+-------------------------+-------------+---------------+----------------+---------------------------------------------------+
|       Expression        | Return type | Pre-condition | Post-condition |                    Description                    |
+=========================+=============+===============+================+===================================================+
| ``val()``               | `Val`       | none          | none           | Constructs a value ``val`` of type `Val`          |
+-------------------------+-------------+---------------+----------------+---------------------------------------------------+
| ``cval()``              | `const Val` | none          | none           | Constructs a value ``cval`` of type `Val`         |
+-------------------------+-------------+---------------+----------------+---------------------------------------------------+
| ``val_cpy(val)``        | `Val`       | none          | none           | Copy-constructs a value ``val_cpy`` from ``val``  |
+-------------------------+-------------+---------------+----------------+---------------------------------------------------+
| ``val_cpy(cval)``       | `Val`       | none          | none           | Copy-constructs a value ``val_cpy`` from ``cval`` |
+-------------------------+-------------+---------------+----------------+---------------------------------------------------+
| ``val_cpy(move(val))``  | `Val`       | none          | none           | Move-constructs a value ``val_cpy`` from ``val``  |
+-------------------------+-------------+---------------+----------------+---------------------------------------------------+
| ``val_cpy = val``       | `Val&`      | none          | none           | Assign ``val`` to ``val_cpy``                     |
+-------------------------+-------------+---------------+----------------+---------------------------------------------------+
| ``val_cpy = cval``      | `Val&`      | none          | none           | Assign ``cval`` to ``val_cpy``                    |
+-------------------------+-------------+---------------+----------------+---------------------------------------------------+
| ``val_cpy = move(val)`` | `Val&`      | none          | none           | Move-assign ``val`` to ``val_cpy``                |
+-------------------------+-------------+---------------+----------------+---------------------------------------------------+



.. _concept-values-ComparableValue:

ComparableValue
###############

Let `RegVal` be a type that models :cpp:concept:`ComparableValue (details) <ComparableValue>`.

Let `RegVal` also models :ref:`Value <concept-values-Value>` and :cpp:concept:`Regular (stl) <stl::Regular>`.

.. _concept-values-ComparableValue-expressions:
.. rubric:: Expressions

Let `RegVal` inherit all valid expressions defined for :ref:`Value <concept-values-Value-expressions>`.

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



.. _concept-values-OrderedValue:

OrderedValue
############

Let `STORegVal` be a type that models :cpp:concept:`OrderedValue (details) <OrderedValue>`.

Let `STORegVal` also models :ref:`ComparableValue <concept-values-ComparableValue>` and :cpp:concept:`StrictTotallyOrdered (stl) <stl::StrictTotallyOrdered>`.

.. _concept-values-OrderedValue-expressions:
.. rubric:: Expressions

Let `STORegVal` inherit all valid expressions defined for :ref:`ComparableValue <concept-values-ComparableValue-expressions>`.

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
