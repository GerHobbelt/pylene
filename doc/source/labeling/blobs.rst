Blobs
=====

Include :file:`<mln/labeling/blobs.hpp>`

.. cpp:namespace:: mln::labeling


.. cpp:function:: \
    template <std::integral Label, Image I> \
    image_ch_value_t<I, Label> blobs(I input, Neighborhood nbh, int& nlabel)


    Labelize the connected components of binary image.


    :tparam Label: The integral numeric type (``std::is_integral_v<Label>`` is ``true``) for the labels,
                   filling the following requirements:
                    * ``Label`` is signed: ``sizeof(Label) <= sizeof(int)``
                    * ``Label`` is unsigned: ``sizeof(Label) < sizeof(int)``
    :param input: Input image
    :param nbh: Neighborhood
    :param nlabel: The number of labels in the image
    :return: An image of label from [0:nlabel] (label 0 is the background label)
    :exception: ``std::runtime_error`` if the number of labels overflows


Notes
-----

Complexity
----------

Linear.
