Transform
=========

Include :file:`<mln/core/ranges/view/transform.hpp>`

.. cpp:namespace:: mln::ranges

#. .. cpp:function:: template <class Func> \
                     auto view::transform(Range r, Func fun)

#. .. cpp:function:: template <class Func> \
                     auto view::transform(Range r, Range r2, Func fun)

   :param r: Input range
   :param r2: Input range
   :param fun: Function to apply on each element

   `transform` returns a view of the input range where *fun* is applied to the elements. The second version applies *fun* on
   elements of *r* as first argument and *r2* as second argument.  This view preserves the following properties:


   =============  ===  ===================   =======================
    Properties    *r*  *transform(r, fun)*   *transform(r, r2, fun)*
   =============  ===  ===================   =======================
   Segmented      X    X                     X
   Forward        X    X                     X
   Reversible     X    X                     X
   Bidirectional  X    X                     X
   Random Access  X    X                     X
   Contiguous     X
   Writable       X    X*                    X*
   =============  ===  ===================   =======================

   (*): only if `fun` is a projection.


   **Examples**
   
   * Multiply all components by 2:
   
      .. code-block:: cpp

         mln::box2d box = {{0,0}, {2,3}};
         mln_foreach(auto p, ranges::view::transform(box, [](auto x) { return x*2; }))
           std::cout << p << ",";


      Outputs:

         .. code-block:: text

            (0,0),(0,2),(0,4),(2,0),(2,2),(2,4),


   * Write on the pair's projected second member:
   
      .. code-block:: cpp

         std::vector<std::pair<int, int>> v = {{0,0}, {1, 1}, {2, 2}};
         mln_foreach(auto& second, ranges::view::transform(v, &std::pair<int, int>::second))
           second = -1;
      
         for(auto&& e : v)
           std::cout << e << ", "


      Outputs:

         .. code-block:: text

            (0,-1),(1,-1),(2,-1),
   

   * Operate on 2 ranges at the same time:

      .. code-block:: cpp

         mln::box2d box = {{0,0}, {2,3}};
         mln::box2d box2 = {{1,1}, {3,4}};
         auto adder = ranges::view::transform(box, box2, [](auto x, auto y) { return x+y; };
         mln_foreach(auto p, adder))
           std::cout << p << ",";


      Outputs:

         .. code-block:: text

            (1,1),(1,3),(1,5),(3,1),(3,3),(3,5),
