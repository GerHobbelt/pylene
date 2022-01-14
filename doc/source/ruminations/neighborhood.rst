About Neighborhood / Structuring Element / Windows
##################################################


Concept:

The three objects represents the same idea: given a location we can access near-by location.

.. topic:: Some mathematical properties:

   * Flat or Non-flat (weighted):
     * Flat: erosion, dilation...
     * Non-flat (weighted): e.g. convolution, non-flat erosion
   * Separable. Square -> 2 lines, Diamond
   * Adaptative (they depend on the localisation)

We would like to iterate over:

* a *point* or a *point iterator*: it gives a *point range*::

    auto p = point or point iterator;
    for (auto q : nbh(p))
      (void) q;

* a *pixel* or a *pixel iterator*: it gives a *pixel range*::

    auto px = pixel or pixel iterator;
    for (auto qx : nbh(px))
      (void) qx;


Optimization issue:

For pixel iterator, we precompute offsets. It has to be declared outside the
outerloop::

  auto px = mln_pixter(f);
  auto neighbors = nbh(px); // Precompute offsets & binds to px
  mln_forall(px) // px advances
    for (auto nx : neighbors)
      (void) nx;

or::

  auto px = mln_pixter(f);
  auto neighbors = nbh(px); // Precompute offsets & binds to px
  mln_forall(px) // px advances
    mln_forall(nx)
      (void) nx;


or::

  auto neighbors = nbh.with(f); // Return a new neighborhood with offsets computed
  for (auto px : f.pixels())
    for (auto nx : neighbors(px))

.. note:: With have to choose a syntax.



Algorithms that run with neighborhood
=====================================

* Erosion / Dilation: with an implementation specific for lines, and for
  separable SE and incremental SE
* Hit or miss: Use two SEs with empty intersection
* Mean filter: Implementation with integral images or incremental updates
* Median / Rank filter: Implementation with incremental updates
* Convolution: can be separable


.. mermaid::

   classDiagram
   WeightedNeighborhood <|-- Neighborhood
   Neighborhood <|-- c4
   Neighborhood <|-- c8
   Neighborhood <|-- hline2d
   Neighborhood <|-- vline2d
   Neighborhood <|-- periodic_line2d
   Neighborhood <|-- rectangle2d
   Neighborhood <|-- ball2d
   Neighborhood <|-- diamond2d_1
   Neighborhood <|-- diamond2d
   Neighborhood <|-- neighb2d



Non-flat Structuring Element / Weighted Neighborhood Concept
============================================================


.. cpp:concept:: template <typename N> WeightedNeighborhood

   Concept for generic weighted neighborbood

   * `SE`: A model of :cpp:concept:`WeightedNeighborhood`
   * `se`: An *constant* instance of `SE`

.. rubric:: `Type definition`
   :class: concept-typedefs

+---------------------+--------------------------------+----------------------------------------------+
|Type                 |           Definition           |Requirements                                  |
+=====================+================================+==============================================+
|`SE::category`       |                                |Convertible to `adaptative_neighborhood_tag`  |
+---------------------+--------------------------------+----------------------------------------------+
|`SE::is_incremental` | Either `std::true_type` or     |                                              |
|                     | `std:false_type`               |                                              |
+---------------------+--------------------------------+----------------------------------------------+
|`SE::is_decomposable`| Either `std::true_type` or     |                                              |
|                     | `std:false_type`               |                                              |
+---------------------+--------------------------------+----------------------------------------------+
|`SE::is_flat`        | Either `std::true_type` or     |                                              |
|                     | `std:false_type`               |                                              |
+---------------------+--------------------------------+----------------------------------------------+

.. rubric:: `Valid expression`
   :class: concept-expr


+----------------------+------------------------+----------------------------------------------------------+
|Expression            | Return Type            | Sementics                                                |
+======================+========================+==========================================================+
| ``se(p)``            | `Range<WeightedPoint>` |Return a :concept:`Forward Range` of points centered in   |
|                      |                        |the point `p` or given by `p` if `p` is a point iterator. |
|                      |                        |                                                          |
+----------------------+------------------------+----------------------------------------------------------+
| ``se(px)``           | `Range<WeightedPixel>` |Return a :concept:`Forward Range` of pixels centered in   |
|                      |                        |the pixel `px` or given by `px` if `px` is a pixel        |
|                      |                        |iterator.                                                 |
+----------------------+------------------------+----------------------------------------------------------+
| ``se.has(p)``        | `boolean`              |True if `p` is in the se.                                 |
+----------------------+------------------------+----------------------------------------------------------+
| ``se.is_centered()`` | `boolean`              |Equivalent to ``se.has(literal::zero)``                   |
+----------------------+------------------------+----------------------------------------------------------+



Structuring Element / Neighborhhod Properties
=============================================

Flat vs Weighted Property
*************************

Structuring Element can be weighted or not (there are so called Flat Structuring
Element). In that case, all there weights are implicitely 1 and `is_flat`
property is set. *Flat Structuring Elements* (resp. *Non-Weighted Neighborhood*) are
called simply named *Structing Elements* (resp. *Neighborhood*).


.. cpp:concept:: template <typename N> Neighborhood

   It refines the :cpp:concept:`WeightedNeighborhood` having the `is_flat` property. It corresponds to flat
   structuring element where all weights are 1.

.. rubric:: `Type definition`
   :class: concept-typedefs

+----------------------------+--------------------+
|Type                        |     Definition     |
|                            |                    |
+============================+====================+
|`SE::is_flat`               | `std::true_type`   |
+----------------------------+--------------------+

Categories
**********

+-------------------------------+-------------------------------------------------------------------+
|Category                       |Operations                                                         |
+===============================+===================================================================+
|Adaptative SE                  |Iterate over SE whose elements depend on the current point         |
|                               |                                                                   |
|                               |                                                                   |
|                               |                                                                   |
+----+--------------------------+-------------------------------------------------------------------+
|    | Constant SE              |The structuring element is said *constant* if it knows size of the |
|    |                          | SE.                                                               |
|    |                          |                                                                   |
|    |                          |                                                                   |
|    |                          |                                                                   |
+----+----+---------------------+-------------------------------------------------------------------+
|    |    | Static SE           |The structuring element is said *static* if it knows size of the SE|
|    |    |                     |at compile time.                                                   |
|    |    |                     |                                                                   |
|    |    |                     |                                                                   |
|    |    |                     |                                                                   |
+----+----+----+----------------+-------------------------------------------------------------------+


The categories are used generally for optimisation purpose. When the numbers of
elements is known, we can precompute the memory offsets and store them. Knowing
size at compile time allows to avoid dynamic allocation for temporay data.


Constant Structuring Element Property
-------------------------------------

.. rubric:: `Type definition`
   :class: concept-typedefs

+----------------------------+----------------------------------------------+
|Type                        |Requirements                                  |
|                            |                                              |
+============================+==============================================+
| `SE::category`             |Convertible to `constant_neighborhood_tag`    |
+----------------------------+----------------------------------------------+


.. rubric:: `Valid expression`
   :class: concept-expr

+--------------------------+------------------------------+------------------------------------------------------+
|Expression                | Return Type                  | Sementics                                            |
+==========================+==============================+======================================================+
| ``se.size()``            | `unsigned`                   | The number of elements in the SE.                    |
+--------------------------+------------------------------+------------------------------------------------------+
| ``se.offsets()``         | `Range<Point>`               | The elements of the SE.                              |
+--------------------------+------------------------------+------------------------------------------------------+
| ``se.before(p)``         | `Range<WeightedPoint>`       | The elements of the SE before the anchor centered in |
|                          |                              | `p`.                                                 |
+--------------------------+------------------------------+------------------------------------------------------+
| ``se.before(px)``        | `Range<WeightedPixel>`       | The elements of the SE before the anchor centered in |
|                          |                              | `px`.                                                |
+--------------------------+------------------------------+------------------------------------------------------+
| ``se.after(p)``          | `Range<WeightedPoint>`       | The elements of the SE after the anchor centered in  |
|                          |                              | `p`.                                                 |
+--------------------------+------------------------------+------------------------------------------------------+
| ``se.after(px)``         | `Range<WeightedPixel>`       | The elements of the SE after the anchor centered in  |
|                          |                              | `px`.                                                |
+--------------------------+------------------------------+------------------------------------------------------+



Static Structuring Element Property
-----------------------------------


.. rubric:: `Type definition`
   :class: concept-typedefs

+--------------------------------+----------------------------------------------+
|Type                            |Requirements                                  |
|                                |                                              |
+================================+==============================================+
| `SE::category`                 |Convertible to `static_neighborhood_tag`      |
+--------------------------------+----------------------------------------------+

.. rubric:: `Valid expression`
   :class: concept-expr

+--------------------+-----------------+------------------------------------------------------------------+
|Expression          | Return Type     | Sementics                                                        |
+====================+=================+==================================================================+
| ``se.size()``      | `unsigned`      |The number of elements in the SE as a *constexpr expression*      |
+--------------------+-----------------+------------------------------------------------------------------+


Incremental Structuring Element Property
****************************************

A SE is said to be *incremental*, if it enables to give the points
that are added or removed to the range given a *basic deplacement* of
the point, e.g. for `point2d`, the basic deplacement is `(0,1)`.  This
is usually used to compute attributes over a sliding SE in linear
time.



.. rubric:: `Type definition`
   :class: concept-typedefs

+--------------------------------+--------------------+----------------------------------------------+
|Type                            |     Definition     |Requirements                                  |
|                                |                    |                                              |
+================================+====================+==============================================+
|`SE::is_incremental`            | `std::true_type`   | A                                            |
+--------------------------------+--------------------+----------------------------------------------+
|`SE::dec_type`                  |                    | A model of :cpp:concept:`SE`                 |
+--------------------------------+--------------------+----------------------------------------------+
|`SE::inc_type`                  |                    | A model of                                   |
|                                |                    | :cpp:concept:`WeightedNeighborhood`          |
+--------------------------------+--------------------+----------------------------------------------+

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

Decomposable Structring Element Property
****************************************

A SE is said to be *decomposable*, if it enables to provides a SE
decomposition. E.g. a square of size n×m is decomposable in a vertical line
of of length *m* and an horizontal line of line *m*.

.. rubric:: `Type definition`
   :class: concept-typedefs

+--------------------------------+--------------------+
|Type                            |     Definition     |
|                                |                    |
+================================+====================+
|`SE::is_decomposable`           | `std::true_type`   |
+--------------------------------+--------------------+


.. rubric:: `Valid expression`
   :class: concept-expr

+--------------------------+-------------------------------+------------------------------------------------------+
|Expression                | Return Type                   | Sementics                                            |
+==========================+===============================+======================================================+
|  ``se.decomposable()``   | `boolean`                     | Check dynamically if the SE is decomposable.         |
|                          |                               |                                                      |
+--------------------------+-------------------------------+------------------------------------------------------+
| ``se.decompose()``       | `Range<WeightedNeighborhood>` | Decompose a SE into a set of non-separable SEs. If   |
|                          |                               | `se` is not separable, it returns a range of one     |
|                          |                               | element: itself.                                     |
+--------------------------+-------------------------------+------------------------------------------------------+
