Hierarchical Watershed
==========

* Include :file:`<mln/morpho/watershed_hierarchy.hpp>`

.. cpp:namespace:: mln::morpho


.. cpp:function:: auto watershed_hierarchy(Image f, AttributeFunction attribute_func, Neighborhood nbh, F dist);

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

The Alpha tree is the tree of :math:`\alpha`-connected components. An :math:`\alpha`-connected component in an image :math:`f`, for a pixel :math:`p`, is defined as

.. math::
    \alpha-CC(p) = \{p\}\ \cup\ \{q\ |\ \text{there exists a path}\ \Pi = \{\pi_0, ..., \pi_n\}\ \text{between}\ p\ \text{and}\ q\ \text{such that}\ d(f(\pi_i), f(\pi_{i+1})) \leq \alpha\}
where :math:`d` is a dissimilarity function between two pixels of the image :math:`f`.

The :math:`\alpha`-connected components form an ordered sequence when :math:`\alpha` is growing, such that for :math:`\alpha_i < \alpha_j`,
:math:`\alpha_i-CC(p) \subseteq \alpha_j-CC(p)`. Thus, the alphatree is the hierarchy where the parenthood relationship represents the inclusion of the
:math:`\alpha`-connected components.

Representation
--------------

.. image:: /figures/morpho/alphatree_repr.svg
    :align: center
    :width: 30%

The ``watershed_hierarchy`` function returns a tree and a node map. The tree has two attributes:

* ``parent``: The parenthood relationship of the node :math:`i`, representing the inclusion relationship of the :math:`\alpha`-connected components.
* ``values``: The value of :math:`\alpha` assigned to a node :math:`i`.

Then, the node map is the relation between a pixel of the image and its related node in the tree, a leaf for the case of the watershed hierarchy.

The image above illustrates the representation of the watershed hierarchy in Pylene, the parenthood relationship being illustrated in arrows, the values of alpha, assigned to each node, being in red, and the relation
between a node of the tree and a pixel of the image being represented by blue dashed lines.

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