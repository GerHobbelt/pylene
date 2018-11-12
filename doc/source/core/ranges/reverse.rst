Reverse
=======

Include :file:`<mln/core/ranges/view/reverse.hpp>`

.. cpp:namespace:: mln::ranges

.. cpp:function:: \
   auto view::reverse(ReversibleRange r)

   :param r: Input range

   If the expression :cpp:expr:`r.reversed()` is valid, then it is
   returned. Otherwise, `r` must be a :cpp:concept:`std::BidirectionalRange`
   and the function returns ``::ranges::view::reverse(rng)``. This view preserves the following properties:

   =============  ===  ================
    Properties    *r*  *reverse(r)*
   =============  ===  ================
   Segmented      X    X
   Forward        X    X
   Reversible     X    X
   Bidirectional  X    X
   Random Access  X    X
   Contiguous     X    X
   =============  ===  ================


   **Examples**::

     mln::box2d box = {{0,0}, {2,3}};
     mln_foreach(auto p, ranges::view::reverse(box))
       std::cout << p << ",";

   Outputs:

   .. code-block:: text

     (1,2),(1,1),(1,0),(0,2),(0,1),(0,0),
