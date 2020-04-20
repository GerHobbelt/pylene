Zip (View)
##########

Include :file:`<mln/core/image/view/zip.hpp>`

.. cpp:namespace:: mln::view

#. .. cpp:function:: auto zip(Image... ima)

    1. Makes a view from `ima` where for each pixel value evals to :cpp:expr:`out(p) = std::make_tuple(ima(p)...)`
    
    :param ima: Input ranges

    .. code::
    
        mln::image2d<int> ima1 = ...;
        mln::image2d<double> ima2 = ...;
        mln::image2d<mln::rgb8> ima3 = ...;
        auto g = mln::view::zip(ima1, ima2, ima3);


+----------------+---------------+----------+---------------+
|                |               | *ima...* | *zip(ima...)* |
+================+===============+==========+===============+
|                | Forward       | X        | X             |
+                +---------------+----------+---------------+
| **Category**   | Bidirectional | X        | X             |
+                +---------------+----------+---------------+
|                | Raw           | X        |               |
+----------------+---------------+----------+---------------+
|                | Writable      | X        | X             |
+                +---------------+----------+---------------+
| **Properties** | Accessible    | X        | X             |
+                +---------------+----------+---------------+
|                | Indexable     | X        | X             |
+----------------+---------------+----------+---------------+


Examples
--------
   
* Traversing both input and output image at the same time:

.. code::

    mln::image2d<uint8_t> input = { {0, 1, 2}, {3, 4, 5} };
    mln::image2d<uint8_t> output{input.domain()};
    mln::fill(output, 0);
    auto zipped_ima = mln::view::zip(input, output);
    for (auto&& [v_in, v_out] : zipped_ima.values())
      v_out = v_in * v_in;
    
    auto vals = zipped_ima.values();
    for (auto r : mln::ranges::rows(vals))
    {
      for (auto&& [_, v_out] : r)
        std::cout << v_out << " ";
      std::cout << std::endl;
    }


Outputs:

.. code-block:: text

    0 1 4 
    9 16 25 
