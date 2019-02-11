RGB (View)
##########

Include :file:`<mln/core/image/view/rgb.hpp>`

.. cpp:namespace:: mln::view

#. .. cpp:function:: auto red(Image ima)
#. .. cpp:function:: auto green(Image ima)
#. .. cpp:function:: auto blue(Image ima)

    1. Makes a view that project the red channel of an rgb colored image.
    2. Makes a view that project the green channel of an rgb colored image.
    3. Makes a view that project the blue channel of an rgb colored image.
    
    :param ima: Input range

    .. code::
    
        mln::image2d<mln::rgb8> ima = ...;
        auto ima_r = mln::view::red(ima);
        auto ima_g = mln::view::green(ima);
        auto ima_b = mln::view::blue(ima);


+----------------+---------------+-------+-----------------------------------+
|                |               | *ima* | *red(ima), green(ima), blue(ima)* |
+================+===============+=======+===================================+
|                | Forward       | X     | X                                 |
+                +---------------+-------+-----------------------------------+
| **Category**   | Bidirectional | X     | X                                 |
+                +---------------+-------+-----------------------------------+
|                | Raw           | X     |                                   |
+----------------+---------------+-------+-----------------------------------+
|                | Writable      | X     | X                                 |
+                +---------------+-------+-----------------------------------+
| **Properties** | Accessible    | X     | X                                 |
+                +---------------+-------+-----------------------------------+
|                | Indexable     | X     | X                                 |
+----------------+---------------+-------+-----------------------------------+


Examples
--------
   
* Set the red channel to max:

.. code::

    mln::image2d<mln::rgb8> ima = {
      { {0, 0, 255}, {0, 0, 255}, {0, 0, 255} },
      { {0, 0, 255}, {0, 0, 255}, {0, 0, 255} },
      { {0, 0, 255}, {0, 0, 255}, {0, 0, 255} }
      };
    auto ima_r = mln::view::red(ima);
    mln::fill(ima_r, 255);
    mln::io::imprint(ima);

Outputs:

.. code-block:: text

    (255, 0, 255) (255, 0, 255) (255, 0, 255)
    (255, 0, 255) (255, 0, 255) (255, 0, 255)
    (255, 0, 255) (255, 0, 255) (255, 0, 255)

