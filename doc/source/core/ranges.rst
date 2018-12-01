======
Ranges
======

Collections of elements are designed in accordance with the `Range TS
<https://en.cppreference.com/w/cpp/experimental/ranges>`_.  The library extends those concepts with two concepts and
helpers functions. Range-related functions and views are in the namespace ``mln::ranges`` whereas concepts are in the
namespace ``mln::concepts``.



.. contents::
   :local:


Concepts
--------


.. cpp:namespace:: mln::concepts


.. _concept-ranges-SegmentedRange:

Segmented (Multidimensional) Range
##################################

Let `SegRng` be a type that models :cpp:concept:`SegmentedRange (details) <SegmentedRange>`.

The `SegRng` also models :cpp:concept:`ForwardRange (stl) <stl::ForwardRange>`.

.. _concept-ranges-SegmentedRange-types:
.. rubric:: Types

Let `SegRng` inherit all types defined for :cpp:concept:`ForwardRange (stl) <stl::ForwardRange>`.

.. _concept-ranges-SegmentedRange-expressions:
.. rubric:: Expressions

Let `SegRng` inherit all valid expressions defined for :cpp:concept:`ForwardRange (stl) <stl::ForwardRange>`.

Let :
    - ``crng`` be an instance of `const SegRng`.

Then we have the following valid expressions :

+-----------------+----------------------------------+---------------+----------------+-------------------------------------------------------------------------------------------------------+
|   Expression    |           Return type            | Pre-condition | Post-condition |                                              Description                                              |
+=================+==================================+===============+================+=======================================================================================================+
| ``crng.rows()`` | ``/* implementation defined */`` | none          | none           | - Return-type models :cpp:concept:`ForwardRange (stl) <stl::ForwardRange>`.                           |
|                 |                                  |               |                | - The `value_type` of the `value_type` of the returned range must be the same as `SegRng::value_type` |
+-----------------+----------------------------------+---------------+----------------+-------------------------------------------------------------------------------------------------------+

.. rubric:: Description

A segmented (multidimentional) range is hierarchical and provides a way to iterate over the last level of the hierarchy.

    .. list-table:: Normal range vs hierarchical range traversal. Left: a single range (hence iterator) traverses a
                    non-contiguous range. Right: a hierarchical range, here a range of contiguous range.

        * - .. image:: /figures/core/linear_rng.svg
                :width: 99%
          - .. image:: /figures/core/segmented_rng.svg
                :width: 99%

Example:

.. code-block:: cpp
    
    for (auto&& row : rng.rows())
      for (auto v : row)
        // Usage of v


.. _concept-ranges-ReversibleRange:

Reversible Range
################
 
Let `RevRng` be a type that models :cpp:concept:`ReversibleRange (details) <ReversibleRange>`.

The `RevRng` also models :cpp:concept:`ForwardRange (stl) <stl::ForwardRange>`.

.. _concept-ranges-ReversibleRange-types:
.. rubric:: Types

Let `RevRng` inherit all types defined for :cpp:concept:`ForwardRange (stl) <stl::ForwardRange>`.

.. _concept-ranges-ReversibleRange-expressions:
.. rubric:: Expressions

Let `RevRng` inherit all valid expressions defined for :cpp:concept:`ForwardRange (stl) <stl::ForwardRange>`.

Let :
    - ``crng`` be an instance of `const RevRng`.

Then we have the following valid expressions :

+---------------------+----------------------------------+---------------+----------------+------------------------------------------------------------------------------+
|     Expression      |           Return type            | Pre-condition | Post-condition |                                 Description                                  |
+=====================+==================================+===============+================+==============================================================================+
| ``crng.reversed()`` | ``/* implementation defined */`` | none          | none           | - Return-type models :cpp:concept:`ForwardRange (stl) <stl::ForwardRange>`.  |
|                     |                                  |               |                | - The returned range's `value_type` must be the same as `SegRng::value_type` |
+---------------------+----------------------------------+---------------+----------------+------------------------------------------------------------------------------+

.. rubric:: Description
 
A reversible range that can be traversed forward and backward. This is not as strict as
:cpp:concept:`BidirectionalRange (stl) <stl::BidirectionalRange>` that requires bidirectional iterators.




Utilities for generic programming
---------------------------------


.. cpp:namespace:: mln::ranges


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




Predifined Views
----------------

.. toctree::

   ranges/reverse
   ranges/transform
   ranges/filter
   ranges/zip
   ranges/zip_with




Concepts detail
---------------

.. toctree::
   
   concepts/ranges
