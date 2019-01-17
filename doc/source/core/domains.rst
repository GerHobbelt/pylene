=======
Domains
=======


Information about the valid set of points for an image.



.. contents::
   :local:



Concepts
--------


.. cpp:namespace:: mln::concepts


.. _concept-domains-Domain:

Domain
######

Let `Dom` be a type that models :cpp:concept:`Domain (details) <Domain>`.

Then `Dom` also models :cpp:concept:`Regular (stl) <stl::Regular>`.

.. _concept-domains-Domain-types:
.. rubric:: Types

Then we can define :

+-------------------+-----------------+----------------------------------------------+
|    Definition     |   Description   |                 Requirement                  |
+===================+=================+==============================================+
| `Dom::point_type` | type of a point | models :ref:`concept-points-Point` concept   |
+-------------------+-----------------+----------------------------------------------+
| `Dom::value_type` | type of a value | - models :ref:`concept-values-Value` concept |
|                   |                 | - non-const                                  |
|                   |                 | - not a reference                            |
+-------------------+-----------------+----------------------------------------------+

.. _concept-domains-Domain-expressions:
.. rubric:: Expressions

Let : 
    - ``cdom`` be an instance of `const Dom`.
    - ``lhs`` be an instance of `const Dom`.
    - ``rhs`` be an instance of `const Dom`.
    - ``p`` be an instance of `Dom::point_type`.

Then we have the following valid expressions :

+------------------+----------------------------------------------------------------------+---------------+----------------+---------------------------------------------------+
|    Expression    |                             Return type                              | Pre-condition | Post-condition |                    Description                    |
+==================+======================================================================+===============+================+===================================================+
| ``lhs == rhs``   | ``bool``                                                             | none          | none           | Compare wether two domains are equal              |
+------------------+----------------------------------------------------------------------+---------------+----------------+---------------------------------------------------+
| ``lhs != rhs``   | ``bool``                                                             | none          | none           | Compare wether two domains are different          |
+------------------+----------------------------------------------------------------------+---------------+----------------+---------------------------------------------------+
| ``cdom.has(p)``  | ``bool``                                                             | none          | none           | Check wether ``p`` belongs to the domain ``cdom`` |
+------------------+----------------------------------------------------------------------+---------------+----------------+---------------------------------------------------+
| ``cdom.empty()`` | ``bool``                                                             | none          | none           | Check wether ``cdom`` is an empty domain          |
+------------------+----------------------------------------------------------------------+---------------+----------------+---------------------------------------------------+
| ``cdom.size()``  | models :cpp:concept:`UnsignedIntegral (stl) <stl::UnsignedIntegral>` | none          | none           | Return the size of the domain.                    |
+------------------+----------------------------------------------------------------------+---------------+----------------+---------------------------------------------------+




Concepts detail
---------------

.. toctree::
   
   concepts/domains
