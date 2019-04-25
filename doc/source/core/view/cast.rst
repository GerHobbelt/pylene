Cast (View)
###########

Include :file:`<mln/core/image/view/cast.hpp>`

.. cpp:namespace:: mln::view

#. .. cpp:function:: template <typename T> auto cast<T>(Image ima)

    1. Makes a view from `ima` where for each pixel value evals to :cpp:expr:`out(p) = static_cast<T>(ima(p))`
    
    :param ima: Input range
    :tparam T: Type to cast to

    .. code::
    
        mln::image2d<double> ima = ...;
        auto g1 = mln::view::cast<int>(ima);

+----------------+---------------+-------+----------------+
|                |               | *ima* | *cast<T>(ima)* |
+================+===============+=======+================+
|                | Forward       | X     | X              |
+                +---------------+-------+----------------+
| **Category**   | Bidirectional | X     | X              |
+                +---------------+-------+----------------+
|                | Raw           | X     |                |
+----------------+---------------+-------+----------------+
|                | Writable      | X     |                |
+                +---------------+-------+----------------+
| **Properties** | Accessible    | X     | X              |
+                +---------------+-------+----------------+
|                | Indexable     | X     | X              |
+----------------+---------------+-------+----------------+


Examples
--------
   
* Double image to int:

.. code::

    mln::image2d<double> input = { {0.1, 1.1, 2.1}, {3.5, 4.5, 5.5} }; 
    auto g = mln::view::cast<int>(input);
    mln::io::imprint(g);

Outputs:

.. code-block:: text

    0 1 2
    3 4 5
