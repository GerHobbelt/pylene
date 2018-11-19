======
Points
======


Way of locating a value inside an image.



.. contents::
   :local:


Concepts
--------


.. cpp:namespace:: mln::core::concepts


.. _concept-points-Point:

Point
#####

Let `Pnt` be a type that models :cpp:concept:`Point (details) <Point>`.

The `Pnt` also models :cpp:concept:`Regular (stl) <stl::Regular>` and :cpp:concept:`StrictTotallyOrdered (stl) <stl::StrictTotallyOrdered>`.

.. _concept-points-Point-expressions:
.. rubric:: Expressions

Let : 
    - ``pnt`` be an instance of `Pnt`.
    - ``pnt_cpy`` be an instance of `Pnt`.
    - ``cpnt`` be an instance of `const Pnt`.
    - ``lhs`` be an instance of `const Pnt`.
    - ``rhs`` be an instance of `const Pnt`.

Then we have the following valid expressions :

+-------------------------+-------------+---------------+----------------+---------------------------------------------------+
|       Expression        | Return type | Pre-condition | Post-condition |                    Description                    |
+=========================+=============+===============+================+===================================================+
| ``pnt()``               | `Pnt`       | none          | none           | Constructs a point ``pnt`` of type `Pnt`          |
+-------------------------+-------------+---------------+----------------+---------------------------------------------------+
| ``cpnt()``              | `const Pnt` | none          | none           | Constructs a point ``cpnt`` of type `Pnt`         |
+-------------------------+-------------+---------------+----------------+---------------------------------------------------+
| ``pnt_cpy(pnt)``        | `Pnt`       | none          | none           | Copy-constructs a point ``pnt_cpy`` from ``pnt``  |
+-------------------------+-------------+---------------+----------------+---------------------------------------------------+
| ``pnt_cpy(cpnt)``       | `Pnt`       | none          | none           | Copy-constructs a point ``pnt_cpy`` from ``cpnt`` |
+-------------------------+-------------+---------------+----------------+---------------------------------------------------+
| ``pnt_cpy(move(pnt))``  | `Pnt`       | none          | none           | Move-constructs a point ``pnt_cpy`` from ``pnt``  |
+-------------------------+-------------+---------------+----------------+---------------------------------------------------+
| ``pnt_cpy = pnt``       | `Pnt&`      | none          | none           | Assign ``pnt`` to ``pnt_cpy``                     |
+-------------------------+-------------+---------------+----------------+---------------------------------------------------+
| ``pnt_cpy = cpnt``      | `Pnt&`      | none          | none           | Assign ``cpnt`` to ``pnt_cpy``                    |
+-------------------------+-------------+---------------+----------------+---------------------------------------------------+
| ``pnt_cpy = move(pnt)`` | `Pnt&`      | none          | none           | Move-assign ``pnt`` to ``pnt_cpy``                |
+-------------------------+-------------+---------------+----------------+---------------------------------------------------+
| ``lhs == rhs``          | `bool`      | none          | none           | Compare wether two points are equal               |
+-------------------------+-------------+---------------+----------------+---------------------------------------------------+
| ``lhs != rhs``          | `bool`      | none          | none           | Compare wether two points are different           |
+-------------------------+-------------+---------------+----------------+---------------------------------------------------+
| ``rhs == lhs``          | `bool`      | none          | none           | Compare wether two points are equal               |
+-------------------------+-------------+---------------+----------------+---------------------------------------------------+
| ``rhs != lhs``          | `bool`      | none          | none           | Compare wether two points are different           |
+-------------------------+-------------+---------------+----------------+---------------------------------------------------+
| ``lhs < rhs``           | `bool`      | none          | none           | Compare order of the two points                   |
+-------------------------+-------------+---------------+----------------+---------------------------------------------------+
| ``lhs > rhs``           | `bool`      | none          | none           | Compare order of the two points                   |
+-------------------------+-------------+---------------+----------------+---------------------------------------------------+
| ``lhs <= rhs``          | `bool`      | none          | none           | Compare order of the two points                   |
+-------------------------+-------------+---------------+----------------+---------------------------------------------------+
| ``lhs >= rhs``          | `bool`      | none          | none           | Compare order of the two points                   |
+-------------------------+-------------+---------------+----------------+---------------------------------------------------+




Predefined points
-----------------

.. toctree::

   points/point2d
   points/point3d




Concepts detail
---------------

.. toctree::
   
   concepts/points
