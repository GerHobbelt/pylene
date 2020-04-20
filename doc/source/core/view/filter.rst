Filter (View)
#############

Include :file:`<mln/core/image/view/filter.hpp>`

.. cpp:namespace:: mln::view

#. .. cpp:function:: auto filter(Image ima, Predicate pred)

    1. Makes a view where each pixel is kept if and only if the predicate :cpp:expr:`pred(ima(p))` is true.
    
    :param ima: Input range
    :param pred: Predicate to apply on pixel values

    .. code::
    
        mln::image2d<int> ima = ...;
        auto g1 = mln::view::filter(ima, [](int v) { return v > 125; });


+----------------+---------------+-------+---------------------+
|                |               | *ima* | *filter(ima, pred)* |
+================+===============+=======+=====================+
|                | Forward       | X     | X                   |
+                +---------------+-------+---------------------+
| **Category**   | Bidirectional | X     | X                   |
+                +---------------+-------+---------------------+
|                | Raw           | X     |                     |
+----------------+---------------+-------+---------------------+
|                | Writable      | X     |                     |
+                +---------------+-------+---------------------+
| **Properties** | Accessible    | X     | X                   |
+                +---------------+-------+---------------------+
|                | Indexable     | X     | X                   |
+----------------+---------------+-------+---------------------+


Examples
--------
   
* Keep all add values:

.. code::

    mln::image2d<int> input = { {0, 1, 2}, {3, 4, 5} };
    auto g = mln::view::transform(input, [](int v) { return v % 2 != 0; });
    mln::io::imprint(g);

Outputs:

.. code-block:: text

    0 1
    3 5
