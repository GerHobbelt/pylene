Transform
=========

Include :file:`<mln/core/ranges/view/transform.hpp>`

.. cpp:namespace:: mln::ranges

.. cpp:function:: \
   template <class Function> \
   auto view::transform(Range r, Function f)

   :param r: Input range
   :param f: Function to apply on each element

   `transform` returns a view of the input range where *f* is applied to
   the elements. This view preserves the following properties:

  
   =============  ===  ================
    Properties    *r*  *transform(r,f)*
   =============  ===  ================
   Segmented      X    X
   Forward        X    X
   Reversible     X    X
   Bidirectional  X    X
   Random Access  X    X
   Contiguous     X
   =============  ===  ================

   **Examples**::

     mln::box2d box = {{0,0}, {2,3}};
     mln_foreach(auto p, ranges::view::transform(box, [](auto x) { return x*2; }))
       std::cout << p << ",";

   Outputs:

   .. code-block:: text

     (0,0),(0,2),(0,4),(2,0),(2,2),(2,4),
