Hierarchical Watershed
==========

* Include :file:`<mln/morpho/watershed_hierarchy.hpp>`

.. cpp:namespace:: mln::morpho


.. cpp:function:: auto watershed_hierarchy(Image input, AttributeFunction attribute_func, Neighborhood nbh, F dist);

    Compute the watershed hierarchy and returns a pair `(tree, node_map)`.
    See :doc:`component_tree` for more information about the representation of tree.
    The implementation is based on [Naj13]_.

    :param input: The input image
    :param attribute_func: The function that define the attribute computation
    :param nbh: The neighborhood relation
    :param dist: The function weighting the edges between two pixels.
    :return: A pair `(tree, node_map)` where *tree* is of type ``component_tree<std::invoke_result_t<F, image_value_t<Image>, image_value_t<Image>>>`` and *node_map* is a mapping between the image pixels and the node of the tree.

    .. rubric:: Requirements

    * ``std::invoke_result_t<F, image_value_t<Image>, image_value_t<Image>>`` is :cpp:concept:`std::totally_ordered`

Definition
----------

Watershed hierarchies, also called attribute-based hierarchies, are constructed by filtering and re-weighting the
Minimum Spanning Tree based on an attribute computation (area, volume, dynamic, etc.) on the Binary Partition Tree. This
process can be imagined as the filling of branches of the Binary Partition Tree with labeled water where the attribute
drives the speed of the filling.

The tree that represent a watershed hierarchy is an Alpha tree, also known as quasi-flat zone hierarchy.
See :doc:`alphatree` for more information.

Example
-------

This example is used to generate the grayscale lena watershed hierarchy by area with a cut at a threshold of 25 below.

    ::

        #include <mln/accu/accumulators/count.hpp>
        #include <mln/accu/accumulators/mean.hpp>
        #include <mln/core/image/ndimage.hpp>
        #include <mln/core/neighborhood/c4.hpp>
        #include <mln/morpho/watershed_hierarchy.hpp>

        mln::image2d<uint8_t> input = ...;

        // Compute the watershed hierarchy by area
        auto area_attribute_func = [](auto tree, auto node_map) -> std::vector<size_t> {
            return tree.compute_attribute_on_points(node_map, mln::accu::features::count<>());
        };
        auto [tree, node_map] = mln::morpho::watershed_hierarchy(input, area_attribute_func, mln::c4);

        // Compute an attribute (for example the average pixels value at each node, as below)
        auto mean = tree.compute_attribute_on_values(node_map, input, mln::accu::accumulators::mean<uint8_t>());

        // Making an horizontal cut of the tree
        const auto threshold = 25; // Threshold of the horizontal cut, that means the lowest alpha in the cut
        auto node_map_cut = tree.horizontal_cut(threshold, node_map); // Return a new node map associated to the cut

        // Labeling the cut with the mean values of each node
        auto out = tree.reconstruct_from(node_map_cut, ranges::make_span(mean)); // Using range-v3 span

    .. list-table::

        * -   .. figure:: /images/watershed_hierarchy_area_color.png

                Watershed hierarchy by area with a cut at a threshold of 100

          -   .. figure:: /images/watershed_hierarchy_area_gray.png

                Watershed hierarchy by area with a cut at a threshold of 25

Notes
-----

Complexity
----------

References
----------

.. [Naj13] Laurent Najman, Jean Cousty, and Benjamin Perret (2013). Playing with kruskal: algorithms for morphological trees in edge-weighted graphs. *International Symposium on Mathematical Morphology and Its Applications to Signal and Image Processing*. Springer, Berlin, Heidelberg. 135-146