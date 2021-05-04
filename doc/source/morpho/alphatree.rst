Alpha Tree
==========

Include :file:`<mln/morpho/alphatree.hpp>`

.. cpp:namespace:: mln::morpho


.. cpp:function:: auto alphatree(Image f, Neighborhood nbh, F dist);

    Compute the alphatree (also known as quasi-flat zone hierarchy) and returns a pair
    `(tree, node_map)`. See :doc:`component_tree` for more information about the
    representation of tree.

    :param input: The input image
    :param nbh: The neighborhood
    :param dist: The function weighting the edges between two pixels
    :return: A pair `(tree, node_map)` where *tree* is of type ``component_tree<std::invoke_result_t<F, image_value_t<Image>, image_value_t<Image>>>`` and *node_map* is a mapping between the image pixels and the node of the tree.

    .. rubric:: Requirements

    * ``std::invoke_result_t<F, image_value_t<Image>, image_value_t<Image>>`` is :cpp:concept:`std::totally_ordered`

    .. rubric:: Example

    ::

        #include <mln/accu/accumulators/mean.hpp>
        #include <mln/morpho/alphatree.hpp>
        #include <mln/core/image/ndimage.hpp>
        #include <mln/core/neighborhood/c4.hpp>
        #include <mln/morpho/cut.hpp> // for horizontal_cut_labelization_from
        mln::image2d<uint8_t> input = ...;

        // Compute the alphatree
        auto [tree, node_map] = mln::morpho::alphatree(input, mln::c4);

        // Compute an attribute (for example the average pixels value at each node, as below)
        auto mean = t.compute_attribute_on_values(node_map, input, mln::accu::accumulators::mean<std::uint8_t>());

        // Making an horizontal cut of the tree
        auto th = ...;
        auto cut = mln::morpho::horizontal_cut_labelization_from(t, node_map, th, mean);


    .. list-table::

        * -   .. figure:: /images/alphatree_cut_color.png

                Cut of the alphatree with a threshold of 10

          -   .. figure:: /images/alphatree_cut_gray.png

                Cut of the alphatree with a threshold of 3

Notes
-----

Complexity
----------


References
----------
