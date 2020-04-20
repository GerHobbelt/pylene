Region Adjacency Graph
======================

Include :file:`<mln/labeling/rag.hpp>`

.. cpp:namespace:: mln::labeling


.. cpp:function:: \
    template <typename Label = int16_t, Image I> \
    image_ch_value_t<I, Label> rag(I input, Neighborhood nbh, Graph& graph, int& nlabel)

    Compute the influence zones of the maxima of a binary image and its Region Adjacency Graph (RAG).

    :tparam Label: The integral numeric type for the labels
    :param input: Input image
    :param nbh: Neighborhood
    :param graph: The RAG to be computed.
    :param nlabel: The number of labels in the image
    :return: An image of labels of the IZ of each maxima (label 0 is the background label)
    :exception: ``std::runtime_error`` if the number of labels overflows


Notes
-----

Complexity
----------

Linear.
