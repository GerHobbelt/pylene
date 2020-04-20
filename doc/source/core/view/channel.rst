Channel (View)
##############

Include :file:`<mln/core/image/view/rgb.hpp>`

.. cpp:namespace:: mln::view

#. .. cpp:function:: auto channel(Image ima, unsigned k)

    2. Makes a view that project the k-th channel of a multi-channelled colored image, when k is only known at runtime.
    
    :param ima: Input range

    .. code::
    
        mln::image2d<mln::rgb8> ima = ...;
        auto ima_red = mln::view::channel(ima, 0);


+----------------+---------------+-------+-------------------+
|                |               | *ima* | *channel(ima, k)* |
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
   
* Set the red channel to max:

.. code::

    mln::image2d<mln::rgb8> ima = {
      { {0, 0, 255}, {0, 0, 255}, {0, 0, 255} },
      { {0, 0, 255}, {0, 0, 255}, {0, 0, 255} },
      { {0, 0, 255}, {0, 0, 255}, {0, 0, 255} }
      };
    auto ima_red = mln::view::channel(ima, 0);
    mln::fill(ima_red, 255);
    mln::io::imprint(ima);

Outputs:

.. code-block:: text

    (255, 0, 255) (255, 0, 255) (255, 0, 255)
    (255, 0, 255) (255, 0, 255) (255, 0, 255)
    (255, 0, 255) (255, 0, 255) (255, 0, 255)
