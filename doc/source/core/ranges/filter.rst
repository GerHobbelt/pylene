Filter
======

Include :file:`<mln/core/ranges/view/filter.hpp>`

.. cpp:namespace:: mln::ranges

.. cpp:function:: template <class Pred> \
                  auto view::filter(Range r, Pred fun)

   :param r: Input range
   :param fun: Predicate to apply on each element

   `filter` returns a view of the input range with elements that satisfies the predicate *fun* i.e.  :math:`\{v \in
   \mathcal{R} \mid fun(v)\}`. This view preserves the following properties:


   =============  ===  ================
    Properties    *r*  *filter(r, fun)*
   =============  ===  ================
   Segmented      X    X
   Forward        X    X
   Reversible     X    X
   Bidirectional  X    X
   Random Access  X    X
   Contiguous     X
   Writable       X    X
   =============  ===  ================

   **Examples**

   * Only white squares of a chessboard:

      .. code-block:: cpp

         box2d box = {{0,0}, {3,4}};
         auto is_white = [](point2d p) { return (p[0] % 2) == (p[1] % 2); };
         mln_foreach(auto p, ranges::view::filter(box, is_white))
           std::cout << p << ",";


      Outputs:

         .. code-block:: text

            (0,0),(0,2),(1,1),(1,3),(2,0),(2,2),


   * Set pair numbers to 0:

      .. code-block:: cpp

         std::vector<int> v = {0, 1, 2, 3, 4, 5, 6};
         auto is_pair = [](int x) { return x % 2 == 0; };
         mln_foreach(auto&& n, ranges::view::filter(v, is_pair))
           v = 0;
      
        for(const auto& e : v)
          std::cout << e << ", ";


      Outputs:

         .. code-block:: text
      
            0, 1, 0, 3, 0, 5, 0,
