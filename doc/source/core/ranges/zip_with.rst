Zip_with
========

Include :file:`<mln/core/ranges/view/zip_with.hpp>`

.. cpp:namespace:: mln::ranges

.. cpp:function:: template <class Func> \
                  auto view::zip_with(Func fun, Ranges... rngs)

   :param fun: Function to use to zip all current element of the ranges
   :param rngs: Input ranges

   `zip_with` returns a view of the input ranges where each element is
   the result of `fun` invoked with each current input elements.
   This view preserves the following properties:

  
   =============  =========  ========================
    Properties    *rngs...*  *zip_with(fun, rngs...)*
   =============  =========  ========================
   Segmented      X          X
   Forward        X          X
   Reversible     X          X
   Bidirectional  X          
   Random Access  X          
   Contiguous     X          
   Writable       X*         X*
   =============  =========  ========================

   (*): onlt if `fun` is a projection.

   
   **Examples**

   * Sum 2 set of coordinates:

      .. code-block:: cpp

         mln::box2d box1 = {{0,0}, {2,3}};
         mln::box2d box2 = {{1,1}, {3,4}};
         auto zipper = [](auto a, auto b){ return a[0]*b[0] + a[1]*b[1]; }
         mln_foreach(auto p, ranges::view::zip_with(zipper, box1, box2))
           std::cout << p << ",";


      Outputs:

         .. code-block:: text

            (0,0),(0,2),(0,6),(2,0),(2,2),(2,6),
