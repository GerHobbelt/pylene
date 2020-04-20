Accumulate
==========

Include :file:`<mln/labeling/accumulate.hpp>`

.. cpp:namespace:: mln::labeling


.. cpp:function:: \
    template <Image I, AccumulatorLike A> \
    std::vector<typename accu::result_of<AccuLike, image_point_t<I>>::type>\
    accumulate(lbl_input, int nlabel, A accu)
    template <Image I, Image J, AccumulatorLike A> \
    std::vector<typename accu::result_of<AccuLike, image_value_t<J>>::type> \
    accumulate_on(I lbl_input, J values, int nlabel, A accu)

    Compute an attribute on points of a label image (1) or on the values of another image (2). 


    :param input: Input image of labels
    :param input (optional): Input image of values
    :param nlabel: The number of labels in the image `input`
    :param accu: The accumulator to compute.
    :return: An array of features of size `nlabel+1` for each component of `input`
    :exception: None


Notes
-----

Complexity
----------

Linear.
