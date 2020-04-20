Alpha Tree
==========

Include :file:`<mln/morpho/alphatree.hpp>`

.. cpp:namespace:: mln::morpho


.. cpp:function:: auto alphatree(Image f, Neighborhood nbh);

    Compute the alphatree (also known as quasi-flat zone hierarchy) and returns a pair
    `(tree, node_map)`. See :doc:`component_tree` for more information about the
    representation of tree. 



    :param input: The input image
    :param nbh: The neighborhood 
    :return: A pair `(tree, node_map)` where *tree* is of type
    `component_tree<image_value_t<I>>` and `node_map` is a mapping from each point to the
    node *id* in the tree.
             
    :precondition: ``f.domain().has(pinf)``

    .. rubric:: Requirements

    * ``image_value_t<I>`` is :cpp:concept:`std::totally_ordered`

    .. rubric:: Example

    ::
        
        #include <mln/morpho/alphatree.hpp>
        #include <mln/core/image/ndimage.hpp>
        #include <mln/core/neighborhood/c4.hpp>

        mln::image2d<uint8_t> input = ...;

        auto [tree, node_map] = mln::morpho::alphatree(input, mln::c4);


Notes
-----

Complexity
----------


References
----------
