Extension (Concept)
===================

Extension (Overview)
--------------------

An extension is the way to extend and define image values outside its
definition domain. It has two objectives:

* It allows the user to define side-effects (border effects) for some algorithms.
* It allows the algorithms to speed up their processing time thanks to
  the padding trick.

Any algorithm is allowed to modify the extenion of an image. This
means that even if the image is constant, e.g. the input of an
algorithm, the algorithm is allowed to write the input's extension.


*  `E`: A model of :cpp:concept:`Extension`
*  `ext`: An instance of `E`

.. rubric:: `Type definition`
   :class: concept-typedefs

+--------------------------------+-------+------------------------------------------------+-------------------+
|Type                            | Abbr  |                   Definition                   |Requirements       |
+================================+=======+================================================+===================+
| `E::support_fill`              |       | either `std::true_type` or `std::false_type`   |                   |
+--------------------------------+-------+------------------------------------------------+-------------------+
| `E::support_mirror`            |       | either `std::true_type` or `std::false_type`   |                   |
+--------------------------------+-------+------------------------------------------------+-------------------+
| `E::support_periodize`         |       | either `std::true_type` or `std::false_type`   |                   |
+--------------------------------+-------+------------------------------------------------+-------------------+

.. rubric:: `Valid expression`
   :class: concept-expr

+----------------------+--------------+-----------------------------------------+-----------------------------------------------+
|Expression            | Return Type  | Precondition                            | Sementics                                     |
+======================+==============+=========================================+===============================================+
|   ``ext.fill(v)``    |              |  `E::support_fill` is `std::true_type`  | Fill the extension with value `v`             |
+----------------------+--------------+-----------------------------------------+-----------------------------------------------+
|   ``ext.mirror()``   |              | `E::support_mirror` is `std::true_type` | Fill the extension by mirrorizing the image.  |
+----------------------+--------------+-----------------------------------------+-----------------------------------------------+
| ``ext.periodize()``  |              |`E::support_periodize` is                | Fill the extension by periodizing the image.  |
|                      |              |`std::true_type`                         |                                               |
+----------------------+--------------+-----------------------------------------+-----------------------------------------------+

