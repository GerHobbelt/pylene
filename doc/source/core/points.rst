======
Points
======


Way of locating a value inside an image.



.. contents::
   :local:


Concepts
--------


.. cpp:namespace:: mln::concepts


.. _concept-points-Point:

Point
#####

Let `Pnt` be a type that models :cpp:concept:`Point (details) <Point>`.

The `Pnt` also models :cpp:concept:`Regular (stl) <stl::Regular>` and :cpp:concept:`StrictTotallyOrdered (stl) <stl::StrictTotallyOrdered>`.

.. _concept-points-Point-expressions:
.. rubric:: Expressions

Let : 
    - ``lhs`` be an instance of `const Pnt`.
    - ``rhs`` be an instance of `const Pnt`.

Then we have the following valid expressions :

+----------------+-------------+---------------+----------------+-----------------------------------------+
|   Expression   | Return type | Pre-condition | Post-condition |               Description               |
+================+=============+===============+================+=========================================+
| ``lhs == rhs`` | ``bool``    | none          | none           | Compare wether two points are equal     |
+----------------+-------------+---------------+----------------+-----------------------------------------+
| ``lhs != rhs`` | ``bool``    | none          | none           | Compare wether two points are different |
+----------------+-------------+---------------+----------------+-----------------------------------------+
| ``lhs < rhs``  | ``bool``    | none          | none           | Compare order of the two points         |
+----------------+-------------+---------------+----------------+-----------------------------------------+
| ``lhs > rhs``  | ``bool``    | none          | none           | Compare order of the two points         |
+----------------+-------------+---------------+----------------+-----------------------------------------+
| ``lhs <= rhs`` | ``bool``    | none          | none           | Compare order of the two points         |
+----------------+-------------+---------------+----------------+-----------------------------------------+
| ``lhs >= rhs`` | ``bool``    | none          | none           | Compare order of the two points         |
+----------------+-------------+---------------+----------------+-----------------------------------------+




Concepts detail
---------------

.. toctree::
   
   concepts/points
