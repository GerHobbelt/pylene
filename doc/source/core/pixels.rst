======
Pixels
======


Information about the couple (Point, Value&) when browsing an image.



.. contents::
   :local:



Concepts
--------


.. cpp:namespace:: mln::concepts


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
    - ``cpix`` be an instance of `const Pix`.
    - ``lhs`` be an instance of `const Pix`.
    - ``rhs`` be an instance of `const Pix`.

Then we have the following valid expressions :

+------------------+----------------+---------------+----------------+-----------------------------------------+
|    Expression    |  Return type   | Pre-condition | Post-condition |               Description               |
+==================+================+===============+================+=========================================+
| ``lhs == rhs``   | ``bool``       | none          | none           | Compare wether two pixels are equal     |
+------------------+----------------+---------------+----------------+-----------------------------------------+
| ``lhs != rhs``   | ``bool``       | none          | none           | Compare wether two pixels are different |
+------------------+----------------+---------------+----------------+-----------------------------------------+
| ``cpix.val()``   | ``reference``  | none          | none           | Access the value of the pixel ``pix``   |
+------------------+----------------+---------------+----------------+-----------------------------------------+
| ``cpix.point()`` | ``point_type`` | none          | none           | Access the point of the pixel ``pix``   |
+------------------+----------------+---------------+----------------+-----------------------------------------+

A pixel really just is a tool that stores a point and the reference to the value accessed by this point.
There is no difference in semantic between a mutable pixel and a constant pixel. The const-ness is carried by the value type.
If the `value_type` is constant then the value returned by `value()` (which returns a `reference`) will not be mutable.




Predefined pixels
-----------------

.. toctree::

   pixels/ndpixel




Concepts detail
---------------

.. toctree::
   
   concepts/pixels
