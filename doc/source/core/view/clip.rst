Clip (View)
###########

Include :file:`<mln/core/image/view/clip.hpp>`

.. cpp:namespace:: mln::view

#. .. cpp:function:: auto clip(Image ima, Domain dom)

    1. Makes a view that clip the original image to a sub-domain. A pixel is kept if and only if :cpp:expr:`dom.has(p.point())` is true.
    
    :param ima: Input range
    :param dom: Input range of point

    .. code::
    
        mln::image2d<int> ima = ...;
        mln::image2d<bool> mask = ...;
        auto g = mln::view::mask(ima, mask);


+----------------+---------------+-------+------------------+
|                |               | *ima* | *clip(ima, dom)* |
+================+===============+=======+==================+
|                | Forward       | X     | X                |
+                +---------------+-------+------------------+
| **Category**   | Bidirectional | X     | X                |
+                +---------------+-------+------------------+
|                | Raw           | X     |                  |
+----------------+---------------+-------+------------------+
|                | Writable      | X     | X                |
+                +---------------+-------+------------------+
| **Properties** | Accessible    | X     | X                |
+                +---------------+-------+------------------+
|                | Indexable     | X     | X                |
+----------------+---------------+-------+------------------+


Examples
--------
   
* Set a sub-domain to 42:

.. code::

    mln::image2d<int> ima = { {0, 1, 2}, {3, 4, 5}, {6, 7, 8} };
    box2d d = {{0, 0}, {1, 1}};
    auto g = mln::view::clip(ima, d);
    mln::fill(g, 42);
    mln::io::imprint(ima);

Outputs:

.. code-block:: text

    42 42 2
    42 42 5
    6  7  8

* Set all odd values to 42:

.. code::

    mln::image2d<int> ima = { {0, 1, 2}, {3, 4, 5}, {6, 7, 8} };
    auto odd_d = mln::where(ima % 2 == 1);
    auto g = mln::view::clip(ima, odd_d);
    mln::fill(g, 42);
    mln::io::imprint(ima);

Outputs:

.. code-block:: text

    0  42 2
    42 4  42
    6  42 8
