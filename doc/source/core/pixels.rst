======
Pixels
======


Information about the couple (Point, Value) when browsing an image.



.. contents::
   :local:



Concepts
--------


.. cpp:namespace:: mln::core::concepts


.. _concept-pixels-Pixel:

Pixel
#####

Let `Pix` be a type that models :cpp:concept:`Pixel (details) <Pixel>`.

Then `Pix` also models :cpp:concept:`Regular (stl) <stl::Regular>`

.. _concept-pixels-Pixel-types:
.. rubric:: Types

Then we can define :

+-------------------+--------------------------------+------------------------------------------------+
|    Definition     |          Description           |                  Requirement                   |
+===================+================================+================================================+
| `Pix::point_type` | type of a point                | models the :ref:`concept-points-Point` concept |
+-------------------+--------------------------------+------------------------------------------------+
| `Pix::value_type` | type of a value                | - models :ref:`concept-values-Value` concept   |
|                   |                                | - non-const                                    |
|                   |                                | - not a reference                              |
+-------------------+--------------------------------+------------------------------------------------+
| `Pix::reference`  | type of a reference on a value |                                                |
+-------------------+--------------------------------+------------------------------------------------+

.. _concept-pixels-Pixel-expressions:
.. rubric:: Expressions

Let : 
    - ``pix`` be an instance of `Pix`.
    - ``pix_cpy`` be an instance of `Pix`.
    - ``cpix`` be an instance of `const Pix`.
    - ``lhs`` be an instance of `const Pix`.
    - ``rhs`` be an instance of `const Pix`.

Then we have the following valid expressions :

+-------------------------+--------------+---------------+----------------+---------------------------------------------------+
|       Expression        | Return type  | Pre-condition | Post-condition |                    Description                    |
+=========================+==============+===============+================+===================================================+
| ``pix()``               | `Pix`        | none          | none           | Constructs a pixel ``pix`` of type `Pix`          |
+-------------------------+--------------+---------------+----------------+---------------------------------------------------+
| ``cpix()``              | `const Pix`  | none          | none           | Constructs a pixel ``cpix`` of type `Pix`         |
+-------------------------+--------------+---------------+----------------+---------------------------------------------------+
| ``pix_cpy(pix)``        | `Pix`        | none          | none           | Copy-constructs a pixel ``pix_cpy`` from ``pix``  |
+-------------------------+--------------+---------------+----------------+---------------------------------------------------+
| ``pix_cpy(cpix)``       | `Pix`        | none          | none           | Copy-constructs a pixel ``pix_cpy`` from ``cpix`` |
+-------------------------+--------------+---------------+----------------+---------------------------------------------------+
| ``pix_cpy(move(pix))``  | `Pix`        | none          | none           | Move-constructs a pixel ``pix_cpy`` from ``pix``  |
+-------------------------+--------------+---------------+----------------+---------------------------------------------------+
| ``pix_cpy = pix``       | `Pix&`       | none          | none           | Assign ``pix`` to ``pix_cpy``                     |
+-------------------------+--------------+---------------+----------------+---------------------------------------------------+
| ``pix_cpy = cpix``      | `Pix&`       | none          | none           | Assign ``cpix`` to ``pix_cpy``                    |
+-------------------------+--------------+---------------+----------------+---------------------------------------------------+
| ``pix_cpy = move(pix)`` | `Pix&`       | none          | none           | Move-assign ``pix`` to ``pix_cpy``                |
+-------------------------+--------------+---------------+----------------+---------------------------------------------------+
| ``lhs == rhs``          | `bool`       | none          | none           | Compare wether two pixels are equal               |
+-------------------------+--------------+---------------+----------------+---------------------------------------------------+
| ``lhs != rhs``          | `bool`       | none          | none           | Compare wether two pixels are different           |
+-------------------------+--------------+---------------+----------------+---------------------------------------------------+
| ``rhs == lhs``          | `bool`       | none          | none           | Compare wether two pixels are equal               |
+-------------------------+--------------+---------------+----------------+---------------------------------------------------+
| ``rhs != lhs``          | `bool`       | none          | none           | Compare wether two pixels are different           |
+-------------------------+--------------+---------------+----------------+---------------------------------------------------+
| ``cpix.value()``        | `reference`  | none          | none           | Access the value of the pixel ``pix``             |
+-------------------------+--------------+---------------+----------------+---------------------------------------------------+
| ``cpix.point()``        | `point_type` | none          | none           | Access the point of the pixel ``pix``             |
+-------------------------+--------------+---------------+----------------+---------------------------------------------------+



Predefined points
-----------------

.. toctree::

   pixels/ndpixel




Concepts detail
---------------

.. toctree::
   
   concepts/pixels
