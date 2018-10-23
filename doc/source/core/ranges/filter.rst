Filter
======

Include :file:`<mln/core/ranges/view/filter.hpp>`

.. cpp:namespace:: mln::ranges

.. cpp:function:: \
   template <class Pred> \
   auto view::filter(Range r, Pred f)

   :param r: Input range
   :param f: Predicate to apply on each element

   `filter` returns a view of the input range with elements that satisfies the predicate *f* i.e.  :math:`\{v \in
   \mathcal{R} \mid f(v)\}` This view preserves the following properties:


   =============  ===  ================
    Properties    *r*  *filter(r,f)*
   =============  ===  ================
   Segmented      X    X
   Forward        X    X
   Reversible     X    X
   Bidirectional  X    X
   Random Access  X    X
   Contiguous     X
   =============  ===  ================

   **Examples**

   Only white squares of a chessboard::

     box2d box = {{0,0}, {3,4}};
     auto is_white = [](point2d p) { return (p[0] % 2) == (p[1] % 2); }
     mln_foreach(auto p, ranges::view::filter(box, is_white))
       std::cout << p << ",";

   Outputs:

   .. code-block:: text

     (0,0),(0,2),(1,1),(1,3),(2,0),(2,2),
