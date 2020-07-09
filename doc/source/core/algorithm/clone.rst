Clone
=====

Include :file:`<mln/core/algorithm/clone.hpp>`

.. cpp:namespace:: mln

.. cpp:function:: InputImage{I} image_concrete_t<I> clone(I src)

    Clone the values from the source image `src` to a destination image which is automatically deduced as the concrete type of the input.
    This primitive is directed at:

    * creating a temporary image with the same values than the input
    * saving the evaluation of a view

    This is equivalent to the following code::

        mln::image_concrete_t<InputImage> dst = src.concretize();
        mln::copy(src, dst);

    The source and destination images should not alias.

    :param src: The source image
    :tparam InputImage: A model of :cpp:concept:`InputImage`
    :tparam OutputImage: A model of :cpp:concept:`OutputImage`
    :return: The concrete image of `src` with the same values


Examples
--------

#. Eval an image view::

    mln::image2d<mln::uint8> f = ...;
    float mean = 10.f;
    float stdv = 2.f;
    auto g = (f - mean) / stdv; 
    mln::image2d<float> f2 = mln::clone(g); // Evaluation of the view

Complexity
----------

Linear in the number of pixels.
