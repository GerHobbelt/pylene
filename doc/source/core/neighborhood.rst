Neighborhood, Sliding Windows and Structuring Elements
######################################################

.. contents::
   :local:


Overview
********

*Neighborhoods*, *Structuring Elements* (SE), *Windows* are objects that
enable to iterate locally on the pixels of an image around a given
anchor. There are all the same concept but differ in semantics:

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


A structuring element 𝑩 is a range generator: given a point `p` (resp. a pixel `px`) it provides the set of points or
pixels :math:`\mathcal{B}(p)` centered (anchored) in `p`. (Note that `p` does not necessary belong to
:math:`\mathcal{B}(p)`. Two kinds of range may be generated:

* :math:`\mathcal{B}(p)` provides a range of points
* :math:`\mathcal{B}(px)` provides a range of pixels from the same image of `px`


For example, the following code iterates over each point in 4x3 rectangle and their 4-connected neighbors::

  box2d box(3, 4);

  for (auto p : box) {
    std::cout << "The neighbors of " << p << " are: " <<
    for (auto q : c4(p))
       std::cout << q << ", ";
    std::cout << "\n";
  }




Concepts
********

.. cpp:namespace:: mln


Structuring Element
-------------------

.. cpp:concept:: template <class N> Neighborhood

                 see :cpp:concept:`StructuringElement`



.. cpp:concept:: template <class SE> StructuringElement


   +-------------------------------+-------------------------------------------------------------------+
   |Categorie                      |Operations                                                         |
   +===============================+===================================================================+
   |Adaptative SE                  |Iterate over SE whose elements are dependant on the current point  |
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

  .. rubric:: Valid Expressions
    :class: concept-expr

  +-------------------+----------------+----------------------------------------------------------------------------------------------------------------------------+
  |    Expression     |  Return type   |                                                         Semantics                                                          |
  +===================+================+============================================================================================================================+
  | ``se(p)``         | *impl-defined* | Return a :cpp:concept:`Forward Range` of points centered in                                                                |
  |                   |                | the point `p`.                                                                                                             |
  +-------------------+----------------+----------------------------------------------------------------------------------------------------------------------------+
  | ``se(px)``        | *impl-defined* | Return a :cpp:concept:`Forward Range` of pixels centered in                                                                |
  |                   |                | the pixel `px`.                                                                                                            |
  +-------------------+----------------+----------------------------------------------------------------------------------------------------------------------------+
  | ``se.before(p)``  | *impl-defined* | Return a :cpp:concept:`Forward Range` of points before `p` (:math:`\{ q ∈ \mathcal{B}(p) ∣ q < p \}`)                      |
  +-------------------+----------------+----------------------------------------------------------------------------------------------------------------------------+
  | ``se.before(px)`` | *impl-defined* | Return a :cpp:concept:`Forward Range` of points before `px` (:math:`\{ qx ∈ \mathcal{B}(px) ∣ qx.point() < px.point() \}`) |
  +-------------------+----------------+----------------------------------------------------------------------------------------------------------------------------+
  | ``se.after(p)``   | *impl-defined* | Return a :cpp:concept:`Forward Range` of points after `p` (:math:`\{ q ∈ \mathcal{B}(p) ∣ q > p \}`)                       |
  +-------------------+----------------+----------------------------------------------------------------------------------------------------------------------------+
  | ``se.after(px)``  | *impl-defined* | Return a :cpp:concept:`Forward Range` of points after `px` (:math:`\{ qx ∈ \mathcal{B}(px) ∣ qx.point() > px.point() \}`)  |
  +-------------------+----------------+----------------------------------------------------------------------------------------------------------------------------+



  .. rubric:: Type definitions
    :class: concept-typedefs

  +--------------------+----------------------------+----------------------------------------------------------+
  |        Type        |         Definition         |                         Comment                          |
  +====================+============================+==========================================================+
  | `SE::category`     |                            | Convertible to `adaptative_neighborhood_tag`             |
  +--------------------+----------------------------+----------------------------------------------------------+
  | `SE::incremental`  | either `std::true_type` or |                                                          |
  |                    | `std:false_type`           |                                                          |
  +--------------------+----------------------------+----------------------------------------------------------+
  | `SE::decomposable` | either `std::true_type` or | [FIXME] This to be deprecated. Concept checking instead. |
  |                    | `std:false_type`           |                                                          |
  +--------------------+----------------------------+----------------------------------------------------------+
  | `SE::separable`    | either `std::true_type` or | [FIXME] This to be deprecated. Concept checking instead. |
  |                    | `std:false_type`           |                                                          |
  +--------------------+----------------------------+----------------------------------------------------------+


Structuring Element Properties
------------------------------


.. cpp:concept:: template <class SE> Decomposable

  [FIXME] To be fixed in code

  A structuring element 𝑩 can be *decomposable* in which case, it has a mathod ``se.decompose()`` that returns a list
  of simpler structuring elements 𝑩₁, 𝑩₂, ..., 𝑩ₙ for which the dilation of an image *f* is:
  
  *f* ⨁ 𝑩 = *f* ⨁ 𝑩₁ ⨁ 𝑩₂ ⨁ ... ⨁ 𝑩ₙ

  The decomposability of a structuring element can be queried *dynamically* with ``se.is_decomposable()``.

  .. cpp:function:: bool is_decomposable() const

    Return *true* if the *se* is decomposable, *false* otherwise. 

  .. cpp:function:: impl_defined decompose() const 

    Return a list of simpler SE. If ``decompose()`` is called while ``is_decomposable()`` returns *false*, a runtime exception is raised.


.. cpp:concept:: template <class SE> Separable

  [FIXME] To be fixed in code

  A structuring element 𝑲 can be *separable* in which case, it has a mathod ``se.separate()`` that returns a list
  of simpler structuring elements 𝑲₁, 𝑲₂, ..., 𝑲ₙ for which the convolution of an image *f* is:

  *f* ★ 𝑲 = *f* ★ 𝑲₁ ★ 𝑲₂ ★ ... ★ 𝑲ₙ


  The separability of a structuring element can be queried *dynamically* with ``se.is_separable()``.

  .. cpp:function:: bool is_separable() const

    Return *true* if the *se* is separable, *false* otherwise. 

  .. cpp:function:: impl_defined separate() const

    Return a list of simpler SE. If ``separate()`` is called while ``is_seperable()`` returns *false*, a runtime exception is raised.


.. cpp:concept:: template <class SE> Incremental

  A SE is said to be *incremental*, if it enables to give the points
  that are added or removed to the range given a *basic deplacement* of
  the point, e.g. for `point2d`, the basic deplacement is `(0,1)`.  This
  is usually used to compute attributes over a sliding SE in linear
  time.



  .. rubric:: `Type definition`
   :class: concept-typedefs

+-------------------+------+------------------+----------------------------------------------+
|       Type        | Abbr |    Definition    |                 Requirements                 |
+===================+======+==================+==============================================+
| `SE::incremental` |      | `std::true_type` |                                              |
+-------------------+------+------------------+----------------------------------------------+
| `SE::dec_type`    |      |                  | A model of :cpp:concept:`StructuringElement` |
+-------------------+------+------------------+----------------------------------------------+
| `SE::inc_type`    |      |                  | A model of :cpp:concept:`StructuringElement` |
+-------------------+------+------------------+----------------------------------------------+

  .. rubric:: `Valid expression`
     :class: concept-expr

+--------------+----------------+-----------------------------------------------------------+
|  Expression  |  Return Type   |                         Sementics                         |
+==============+================+===========================================================+
| ``se.inc()`` | `SE::inc_type` | A SE equivalent to :math:`\Delta\mathcal{B}^+(p) =        |
|              |                | \mathcal{B}(p) \setminus (\mathcal{B}(p) \cap             |
|              |                | \mathcal{B}(\mathrm{prev}))`                              |
+--------------+----------------+-----------------------------------------------------------+
| ``se.dec()`` | `SE::dec_type` | A SE `s` equivalent to :math:`\Delta\mathcal{B}^-(p) =    |
|              |                | \mathcal{B}(\mathrm{prev}) \setminus (\mathcal{B}(p) \cap |
|              |                | \mathcal{B}(\mathrm{prev}))`                              |
+--------------+----------------+-----------------------------------------------------------+






Predefined Neighborhoods
************************

  .. toctree::
    :maxdepth: 1

    neighborhood/c4
    neighborhood/c8


Predefined Structuring Elements
*******************************

  .. toctree::
   :maxdepth: 1

   se/disc
   se/rectangle
   se/periodic_lines


Tools to build custom Neighborhoods and Structuring Elements
************************************************************

[FIXME]