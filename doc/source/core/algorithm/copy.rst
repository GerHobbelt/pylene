Copy
====

Include :file:`<mln/core/algorithm/copy.hpp>`

.. cpp:namespace:: mln

.. cpp:function:: void copy(InputImage src, OutputImage dst)

    Copy the values from the source image `src` to destination image `dst` without considering the location. To consider the domain, see :cpp:func:`mln::paste`.
    This is equivalent to the following code::

        for (auto&& [vin,vout] : ranges::view::zip(src.values(), dst.values())
            vout = vin;

    The source and destination images should not alias.

    :param src: The source image
    :param dst: The destination image
    :tparam InputImage: A model of :cpp:concept:`InputImage`
    :tparam OutputImage: A model of :cpp:concept:`OutputImage`

    

Examples
--------

#. Copy the red component into the green component::

    mln::image2d<mln::rgb8> f = ...;
    mln::copy(mln::view::red(f), mln::view::green(f));

#. Copy a upper left part of an image into the lower right part::

    mln::image2d<mln::rgb8> f(20,20);
    mln::box2d tl = {{0,0}, {10,10}};
    mln::box2d br = {{10,10}, {20,20}};
    mln::copy(mln::view::clip(f, tl), mln::view::clip(f, br)); // or mln::copy(f | tl, f | br);
 
Complexity
----------

Linear in the number of pixels.
 