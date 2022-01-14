Fill
====

Include :file:`<mln/core/algorithm/fill.hpp>`

.. cpp:namespace:: mln

.. cpp:function:: template <class V> void fill(OutputImage ima, const V& value)

    Assign the value `value` to each pixel of the image. This is equivalent to the following code::

        for (auto& vout : ima.values())
            vout = value;

    This function has a parallel implementation, see following section for an example.

    :param ima: The image to be filled.
    :param value: The value to fill with.
    :tparam V: Must be convertible to :cpp:expr:`image_value_t<OutputImage>`
    :tparam OutputImage: A model of :cpp:concept:`OutputImage`


    

Examples
--------

#. Set the pixels to a color::

    mln::image2d<mln::rgb8> f = ...;
    mln::fill(f, mln::rgb8{255,0,0});

#. Set the red component of a RGB image::

    mln::image2d<mln::rgb8> f = ...;
    mln::fill(mln::view::red(f), 69);

#. Using parallel fill to set the pixels to a color::

    mln::image2d<mln::rgb8> f = ...;
    mln::parallel::fill(f, mln::rgb8{255,0,0});
 
Complexity
----------

Linear in the number of pixels.
 