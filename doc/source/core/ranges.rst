Ranges
######

Collections of elements are designed in accordance with the `Range TS
<https://en.cppreference.com/w/cpp/experimental/ranges>`_.  The library extends those concepts with two concepts and
helpers functions. Range-related functions, concepts and views are in the namespace ``mln::ranges``



.. contents::
   :local:


.. cpp:namespace:: mln::ranges

Concepts
********

Segmented (Multidimensional) Range
==================================

.. cpp:concept:: template <typename R> SegmentedRange

    A segmented (multidimentional) range is hierarchical and provides a way to iterate over the last level of the hierarchy.


    .. list-table:: Normal range vs hierarchical range traversal. Left: a single range (hence iterator) traverses a
                    non-contiguous range. Right: a hierarchical range, here a range of contiguous range.

        * - .. image:: /figures/core/linear_rng.svg
                :width: 99%
          - .. image:: /figures/core/segmented_rng.svg
                :width: 99%



    .. rubric:: valid expressions

    - :cpp:expr:`r.rows()`, with return type a range of range of :cpp:expr:`R::value_type`.


Example::

    for (auto&& row : rng.rows())
        for (auto v : row)
            // Usage of v


Reversible Range
================

.. cpp:concept:: template <typename R> ReversibleRange

    A reversible range that can be traversed forward and backward. This is not
    as strict as :cpp:concept:`BidirectionalRange` that requires bidirectional
    iterators).

    .. rubric:: valid expression

    - :cpp:expr:`mln::ranges::view::reverse(r)` where r is a :cpp:concept:`ReversibleRange`.




Utilities for generic programming
*********************************

.. cpp:function:: auto rows(Range rng)

   :param rng: Input range

   Generic util for iterating row-wise over a multidimentional or a non-multidimentional range for performance.  If
   `rng` is multidimensional, it returns :cpp:expr:`rng.rows()`, otherwise it returns the singleton
   :cpp:expr:`::ranges::single(rng)`.


   **Example**::

      for (auto r : ranges::rows(rng))
        for (auto v : r)
          // Usage of v


.. c:macro:: mln_foreach

   This macro eases the usage of multidimensional ranges for generic programming and efficient iteration.  The syntax is
   :cpp:expr:`mln_foreach(decl_expr, rng_expr)`. If :cpp:expr:`decl_expr` uses commas, it has to be parenthesized.


   .. rubric:: Examples

   **Simple usage**::

     mln_foreach(auto v, rng)
       // Usage of v

   **Usage with bind expressions**::

     mln_foreach((auto [v1, v2]), mln::ranges::zip(rng1, rng2))
       // Usage of v1 and v2


Views
*****

.. toctree::

   ranges/reverse
   ranges/transform
