Zip
===

Include :file:`<mln/core/ranges/view/zip.hpp>`

.. cpp:namespace:: mln::ranges

.. cpp:function:: \
   auto view::zip(Ranges... rngs)

   :param rngs: Input ranges

   `zip` returns a view of the input ranges where each element is
   a tuple of the input elements. This view preserves the following properties:

  
   =============  =========  ================
    Properties    *rngs...*  *zip(rngs...)*
   =============  =========  ================
   Segmented      X          X
   Forward        X          X
   Reversible     X          
   Bidirectional  X          
   Random Access  X          
   Contiguous     X          
   =============  =========  ================

   **Examples**::

     mln::box2d box1 = {{0,0}, {2,3}};
     mln::box2d box2 = {{1,1}, {3,4}};
     mln_foreach(auto [p1, p2], ranges::view::zip(box1, box2))
       std::cout << "[" << p1 << ", " << p2 << "]\n";

   Outputs:

   .. code-block:: text

     [(0,0), (1,1)]
     [(0,1), (1,2)]
     [(0,2), (1,3)]
     [(1,0), (2,1)]
     [(1,1), (2,2)]
     [(1,2), (2,3)]
