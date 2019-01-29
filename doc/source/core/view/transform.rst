Transform (View)
################

Include :file:`<mln/core/image/view/transform.hpp>`

.. cpp:namespace:: mln::view

#. .. cpp:function:: auto transform(Image ima, UnaryFunction f)
#. .. cpp:function:: auto transform(Image ima, Image ima2, BinaryFunction f)

    1. Makes a view from `ima` where for each pixel value evals to `out(p) = f(ima(p))`
    2. Makes a view from `ima` where for each pixel value evals to `out(p) = f(ima(p),ima2(p))`. For this overload, the domain of both image must be equal.
    

    :param ima: Input range
    :param ima2: Input range
    :param f: Function to apply on pixel values



.. warning:: The function f must not create dangling references on the input.

    .. code::
    
        mln::image2d<mln::rgb8> ima = ...;
        auto g1 = mln::view::transform(ima, [](rgb8 v) { return v; }); // OK: identity but creates a non-mutable image 
        auto g2 = mln::view::transform(g1, [](const rgb8& v) -> const uint8& { return v[0]; }); // KO: create a dangling reference to g1(p)



+----------------+---------------+------------+---------------------+---------------------------+
|                |               | *ima,ima2* | *transform(ima, f)* | *transform(ima, ima2, f)* |
+================+===============+============+=====================+===========================+
|                | Forward       | X          | X                   | X                         |
+                +---------------+------------+---------------------+---------------------------+
| **Category**   | Bidirectional | X          | X                   | X                         |
+                +---------------+------------+---------------------+---------------------------+
|                | Raw           | X          |                     |                           |
+----------------+---------------+------------+---------------------+---------------------------+
|                | Writable      | X          | X*                  | X*                        |
+                +---------------+------------+---------------------+---------------------------+
| **Properties** | Accessible    | X          | X                   | X                         |
+                +---------------+------------+---------------------+---------------------------+
|                | Indexable     | X          | X                   | X                         |
+----------------+---------------+------------+---------------------+---------------------------+


(*): only if `f` is a projection.


Examples
--------
   
* Multiply all values by 2:

.. code::

    mln::image2d<uint8_t> input = { {0,1,2}, {3,4,5} }; 
    auto g = mln::view::transform(input, [](mln::uint8 v) { return v * 2; });
    mln::io::imprint(g);

Outputs:

.. code-block:: text

    0 2 4
    6 8 10

* Write on the red projected member:

.. code::

    mln::image2d<mln::rgb8> input = {{{0,0,0}, {1,1,1}, {2,2,2}}, {{3,3,3}, {4,4,4}, {5,5,5}}}; 
    auto g = mln::view::transform(input, [](mln::rgb8& x) -> uint8_t& { return x[0]; });
    mln::fill(g, 42);

Outputs:

.. code-block:: text

    [42,0,0] [42,1,1] [42,2,2]
    [42,3,3] [42,4,4] [42,5,5]


* Transform two images with a binary function (alpha blending):

.. code-block:: cpp

    mln::image2d<uint8_t> input1 = ...; 
    mln::image2d<uint8_t> input2 = ...; 
    auto f = [](uint8_t x, uint8_t y) { return alpha * x + (1 - alpha) * y; };
    auto g = mln::view::transform(input1, input2, f);
