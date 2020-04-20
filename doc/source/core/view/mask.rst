Mask (View)
###########

Include :file:`<mln/core/image/view/mask.hpp>`

.. cpp:namespace:: mln::view

#. .. cpp:function:: auto mask(Image ima, BoolImage m)

    1. Makes a view where each pixel is kept if and only if the masked boolean :cpp:expr:`m(p)` is true.
    
    :param ima: Input range
    :param mask: Input range

    .. code::
    
        mln::image2d<int> ima = ...;
        mln::image2d<bool> m = ...;
        auto g = mln::view::mask(ima, m);


+----------------+---------------+-------+-------------------+
|                |               | *ima* | *mask(ima, mask)* |
+================+===============+=======+===================+
|                | Forward       | X     | X                 |
+                +---------------+-------+-------------------+
| **Category**   | Bidirectional | X     | X                 |
+                +---------------+-------+-------------------+
|                | Raw           | X     |                   |
+----------------+---------------+-------+-------------------+
|                | Writable      | X     | X                 |
+                +---------------+-------+-------------------+
| **Properties** | Accessible    | X     | X                 |
+                +---------------+-------+-------------------+
|                | Indexable     | X     | X                 |
+----------------+---------------+-------+-------------------+


Examples
--------
   
* Set all odd values to 42:

.. code::

    mln::image2d<int> ima = { {0, 1, 2}, {3, 4, 5} };
    mln::image2d<bool> m = ima % 2 == 1;
    auto g = mln::view::mask(ima, m);
    mln::fill(g, 42);
    mln::io::imprint(ima);

Outputs:

.. code-block:: text

    0  42 2
    42 4  42
