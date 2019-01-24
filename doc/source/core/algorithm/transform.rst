Transform
=========

Include :file:`<mln/core/algorithm/transform.hpp>`

.. cpp:namespace:: mln

#. .. cpp:function:: void transform(InputImage in, OutputImage out, UnaryFunction f)

#. .. cpp:function:: image_ch_value_t<InputImage, R> transform(InputImage in, UnaryFunction f)


    1) Applies the function `f` to every value of `in` and stores the result in `out`.
    2) Same as (1), but `out` is deduced from input with ``R = std::decay_t<std::invoke_result_t<UnaryFunction, image_reference_t<InputImage>>>``.

    This is equivalent to the following code::

        for (auto&& [vin, vout] : ranges::zip(in.values(), out.values())
            vout = f(vin);

    :param in: The input image.
    :param out: The output image.
    :param f: The function to apply on values.
    :tparam InputImage: A model of :cpp:concept:`InputImage`
    :tparam OutputImage: A model of :cpp:concept:`OutputImage`

    .. rubric:: Preconditions

    *  ``in.domain() == out.domain()`` (1)

    .. note:: This function is intended for out-of-place computation. For in-place transformation, see :cpp:func:`mln::for_each`.
    

Examples
--------

#. Add one to the values of an image::

    mln::image2d<uint8_t> ima = { {1, 2, 3}, {4, 5, 6} };
    auto out = mln::transform(ima, [](uint8_t x) -> uint8_t { return x + 1; });

#. Get the first channel of a bi-band image::

    using V = std::pair<uint8_t, uint8_t>;
    mln::image2d<V> ima = {{{1, 0}, {2, 0}, {3, 0}}, {{4, 0}, {5, 0}, {6, 0}}};

    mln::image2d<uint8_t> out = mln::transform(ima, &V::first);
  
 
Complexity
----------

Linear in the number of pixels.
 