Clone
=====

Include :file:`<mln/core/algorithm/clone.hpp>`

.. cpp:namespace:: mln

.. cpp:function:: image_concrete_t<InputImage> clone(InputImage src)

    Clone the values from the source image `src` to a destination image which is automatically deduced as the concrete type of the input.
    This primitive is directed at:

    * creating a temporary image with the same values than the input
    * saving the evaluation of a view

    This is equivalent to the following code::

        mln::image_concrete_t<InputImage> dst = src.concretize();
        mln::copy(src, dst);

    The source and destination images should not alias.
    This function has a parallel implementation, see following section for an example.

    :param src: The source image
    :tparam InputImage: A model of :cpp:concept:`InputImage`
    :return: The concrete image of `src` with the same values



Examples
--------

#. Eval an image view::

    mln::image2d<mln::uint8> f = ...;
    float mean = 10.f;
    float stdv = 2.f;
    auto g = (f - mean) / stdv; 
    mln::image2d<float> f2 = mln::clone(g); // Evaluation of the view

#. Using parallel clone to evaluate an image view::

    mln::image2d<mln::uint8> f = ...;
    float mean = 10.f;
    float stdv = 2.f;
    auto g = (f - mean) / stdv; 
    mln::image2d<float> f2 = mln::parallel::clone(g); // Evaluation of the view


Complexity
----------

Linear in the number of pixels.
