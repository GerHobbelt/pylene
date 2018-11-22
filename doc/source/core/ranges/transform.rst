Transform
=========

Include :file:`<mln/core/ranges/view/transform.hpp>`

.. cpp:namespace:: mln::ranges

#. .. cpp:function:: \
      template <class Function> \
      auto view::transform(Range r, Function f)

#. .. cpp:function:: \
      template <class Function> \
      auto view::transform(Range r, Range r2, Function f)

      :param r: Input range
      :param r2: Input range
      :param f: Function to apply on each element

      `transform` returns a view of the input range where *f* is applied to the elements. The second version applies *f* on
      elements of *r* as first argument and *r2* as second argument.  This view preserves the following properties:


      =============  ===  ================   ===================
       Properties    *r*  *transform(r,f)*   *transform(r,r2,f)*
      =============  ===  ================   ===================
      Segmented      X    X                  X
      Forward        X    X                  X
      Reversible     X    X                  X
      Bidirectional  X    X                  X
      Random Access  X    X                  X
      Contiguous     X
      =============  ===  ================   ===================

      **Examples**::

        mln::box2d box = {{0,0}, {2,3}};
        mln_foreach(auto p, ranges::view::transform(box, [](auto x) { return x*2; }))
          std::cout << p << ",";

      Outputs:

      .. code-block:: text

        (0,0),(0,2),(0,4),(2,0),(2,2),(2,4),
