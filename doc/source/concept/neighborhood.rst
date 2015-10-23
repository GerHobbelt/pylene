Neighborhood/Window/SE (Concept)
================================

.. contents::
   :local:

.. concept:: SE

Structuring Element (Overview)
------------------------------

Neighborhoods, Structuring Elements (SE), Windows are all objects that
enable to iterate locally on the pixels of an image around a given
point. There are all the same concept but differ in semantics:

Neighborhoods
  Neighborhoods are usually used to define the connectivity of the
  image, e.g. for 2D images, it is usually the 4-connectivity or the
  8-connectivity.

Structuring Elements
  *Structuring Element* is the common term in Mathematical Morphology to
  define a structure

Windows
  *Sliding window* is the common term to designate a square *Structuring
  Element*. It generally refers to 2D grid.


A structuring element 𝑩 is a range generator: given a point `p`
(resp. a pixel `px`) it provides the set of points or pixels
:math:`\mathcal{B}(p)` centered in `p`. Two kinds of range may be
generated:

* :math:`\mathcal{B}(p)` provides a range of points
* :math:`\mathcal{B}(px)` provides a range of pixels from the same image of `px`

Depending of the nature of `p` or `px`, the range will "follow" the
binded point/pixel:

* if `p` or `px` is an *rvalue*, the point or the pixel is copied and
  the generated range is contant.
* if `p` or `px` is a *lvalue*, a reference to the point or the pixel
  is taken. The range is bounded to it. Any modification to `p` or `px`,
  alters the generated range. It invalidates any iterator currently on
  the range::

    box2d box = { {0,0}, {3,4} };
    point2d p;
    mln_iter(q, c4(p));

    mln_foreach(p, box)
      mln_forall(q)
        p = point2d{10,10};
        std::cout << *q; // unspecified behavior



Overview of the  concept.

+-------------------------------+-------------------------------------------------------------------+
|Categorie                      |Operations                                                         |
+===============================+===================================================================+
|Adapatative SE                 |Iterate over SE whose elements are dependant on the current point  |
|                               |                                                                   |
|                               |                                                                   |
|                               |                                                                   |
+----+--------------------------+-------------------------------------------------------------------+
|    | Dynamic SE               |Iterate over SE whose elements are constant but known at run time. |
|    |                          |                                                                   |
|    |                          |                                                                   |
|    |                          |                                                                   |
+----+----+---------------------+-------------------------------------------------------------------+
|    |    | Static SE           |Iterate over SE whose elements are constant, size known at compile |
|    |    |                     |time                                                               |
|    |    |                     |                                                                   |
|    |    |                     |                                                                   |
+----+----+----+----------------+-------------------------------------------------------------------+
|    |    |    | Constant SE    |Iterate over SE whose elements are kwown at compile time.          |
|    |    |    |                |                                                                   |
|    |    |    |                |                                                                   |
+----+----+----+----------------+-------------------------------------------------------------------+

.. concept:: Adaptative SE

Adaptative Structuring Element
------------------------------

* `SE`: A model of :concept:`Adaptative SE`
* `se`: An *constant* instance of `SE`


.. rubric:: `Type definition`
   :class: concept-typedefs

+---------------------+--------------------------------+----------------------------------------------+
|Type                 |           Definition           |Requirements                                  |
+=====================+================================+==============================================+
|`SE::category`       |                                |Convertible to `adaptative_neighborhood_tag`  |
+---------------------+--------------------------------+----------------------------------------------+
|`SE::is_incremental` |either `std::true_type` or      |                                              |
|                     |`std:false_type`                |                                              |
+---------------------+--------------------------------+----------------------------------------------+
|                     |                                |                                              |
+---------------------+--------------------------------+----------------------------------------------+

.. rubric:: `Valid expression`
   :class: concept-expr


+----------------------+----------------+----------------------------------------------------------+
|Expression            | Return Type    | Sementics                                                |
+======================+================+==========================================================+
| ``se(p)``            | *undefined*    |Return a :concept:`Forward Range` of points centered in   |
|                      |                |the point `p` or given by `p` if `p` is a point iterator. |
|                      |                |                                                          |
+----------------------+----------------+----------------------------------------------------------+
| ``se(px)``           | *undefined*    |Return a :concept:`Forward Range` of pixels centered in   |
|                      |                |the pixel `px` or given by `px` if `px` is a pixel        |
|                      |                |iterator.                                                 |
+----------------------+----------------+----------------------------------------------------------+
| ``se.has(p)``        | `boolean`      |True if `p` is in the se.                                 |
+----------------------+----------------+----------------------------------------------------------+
| ``se.is_centered()`` | `boolean`      |Equivalent to ``se.has(literal::zero)``                   |
+----------------------+----------------+----------------------------------------------------------+
|                      |                |                                                          |
+----------------------+----------------+----------------------------------------------------------+

.. concept:: Dynamic SE

Dynamic Structuring Element
---------------------------

The dynamic structuring element refines the :concept:`Adaptative
SE`. The SE is not locally dependant to the current location and stays
the same anywhere in the domain.


.. rubric:: `Type definition`
   :class: concept-typedefs

+--------------------------------+-------+-----------------+----------------------------------------------+
|Type                            | Abbr  |   Definition    |Requirements                                  |
|                                |       |                 |                                              |
+================================+=======+=================+==============================================+
|`SE::category`                  |       |                 |Convertible to `dynamic_neighborhood_tag`     |
+--------------------------------+-------+-----------------+----------------------------------------------+

.. rubric:: `Valid expression`
   :class: concept-expr

+----------------------+-------------------------+----------------+------------------------------------------------------+
|Expression            | Return Type             | Precondition   | Sementics                                            |
+======================+=========================+================+======================================================+
| ``se.size()``        | `unsigned`              |                | The number of elements in the SE.                    |
+----------------------+-------------------------+----------------+------------------------------------------------------+

.. concept:: Static SE

Static Structuring Element
--------------------------

The static structuring element refines the :concept:`Dynamic SE`. The
only difference is that the static SE known the size of the SE at
compile time.

.. rubric:: `Type definition`
   :class: concept-typedefs

+--------------------------------+-------+-----------------+----------------------------------------------+
|Type                            | Abbr  |   Definition    |Requirements                                  |
|                                |       |                 |                                              |
+================================+=======+=================+==============================================+
|`SE::category`                  |       |                 |Convertible to `static_neighborhood_tag`      |
+--------------------------------+-------+-----------------+----------------------------------------------+

.. rubric:: `Valid expression`
   :class: concept-expr

+--------------------+-----------------+------------------------------------------------------------------+
|Expression          | Return Type     | Sementics                                                        |
+====================+=================+==================================================================+
| ``se.size()``      | `unsigned`      |The number of elements in the SE as a *constant expression*       |
|                    |                 |                                                                  |
+--------------------+-----------------+------------------------------------------------------------------+

.. concept:: Constant SE

Constant Structuring Element
----------------------------

The constant structuring element refines the :concept:`Static SE`. The
only difference is that the constant SE known the size and the elemnts
of the SE at compile time.

.. rubric:: `Type definition`
   :class: concept-typedefs

+--------------------------------+-------+--------------------+----------------------------------------------+
|Type                            | Abbr  |     Definition     |Requirements                                  |
|                                |       |                    |                                              |
+================================+=======+====================+==============================================+
|`SE::category`                  |       |                    |Convertible to `constant_neighborhood_tag`    |
+--------------------------------+-------+--------------------+----------------------------------------------+

.. rubric:: `Valid expression`
   :class: concept-expr

+----------------------+----------------+-----------------------------------------------------------------+
|Expression            | Return Type    | Sementics                                                       |
+======================+================+=================================================================+
| ``se.has(p)``        | `boolean`      |      True if `p` is in the se as a *constant expression*.       |
+----------------------+----------------+-----------------------------------------------------------------+
| ``se.is_centered()`` | `boolean`      |Equivalent to ``se.has(literal::zero)`` as a *constant           |
|                      |                |expression*.                                                     |
+----------------------+----------------+-----------------------------------------------------------------+

.. concept:: Incremental SE

Incremental Structuring Element
---------------------------------

A SE is said to be *incremental*, if it enables to give the points
that are added or removed to the range given a *basic deplacement* of
the point, e.g. for `point2d`, the basic deplacement is `(0,1)`.  This
is usually used to compute attributes over a sliding SE in linear
time.



.. rubric:: `Type definition`
   :class: concept-typedefs

+--------------------------------+-------+--------------------+----------------------------------------------+
|Type                            | Abbr  |     Definition     |Requirements                                  |
|                                |       |                    |                                              |
+================================+=======+====================+==============================================+
|`SE::is_incremental`            |       | `std::true_type`   |                                              |
+--------------------------------+-------+--------------------+----------------------------------------------+
|`SE::dec_type`                  |       |                    | A model of :concept:`SE`                     |
+--------------------------------+-------+--------------------+----------------------------------------------+
|`SE::inc_type`                  |       |                    | A model of :concept:`SE`                     |
+--------------------------------+-------+--------------------+----------------------------------------------+

.. rubric:: `Valid expression`
   :class: concept-expr

+----------------------+-------------------------+---------------------------------------------------------+
|Expression            | Return Type             | Sementics                                               |
+======================+=========================+=========================================================+
| ``se.inc()``         | `SE::inc_type`          |A SE equivalent to :math:`\Delta\mathcal{B}^+(p) =       |
|                      |                         |\mathcal{B}(p) \setminus (\mathcal{B}(p) \cap            |
|                      |                         |\mathcal{B}(\mathrm{prev}))`                             |
+----------------------+-------------------------+---------------------------------------------------------+
| ``se.dec()``         | `SE::dec_type`          |A SE `s` equivalent to :math:`\Delta\mathcal{B}^-(p) =   |
|                      |                         |\mathcal{B}(\mathrm{prev}) \setminus (\mathcal{B}(p) \cap|
|                      |                         |\mathcal{B}(\mathrm{prev}))`                             |
+----------------------+-------------------------+---------------------------------------------------------+

