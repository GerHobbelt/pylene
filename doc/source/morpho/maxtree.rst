Max-tree
========

Include :file:`<mln/morpho/maxtree.hpp>`

.. cpp:namespace:: mln::morpho


.. cpp:function:: Image{I} auto maxtree(I input, Neighborhood nbh)
                  Image{I} auto mintree(I input, Neighborhood nbh)

    Compute a max-tree or a min-tree and returns a pair (tree, node_map). See :doc:`component_tree` for
    more information about the representation of tree. The value_type
    of the image must be an unsigned integral. 

    :param input: The input image
    :param nbh: The neighborhood to consider
    :return: A pair `(tree, node_map)` where *tree* is of type `component_tree<image_value_t<I>>` and
             

    .. rubric:: Requirements

    * ``image_value_t<I>`` is a :cpp:concept:`UnsignedIntegral`
    * ``N`` is model of :cpp:concept:`Neighborhood`

    .. rubric:: Example

    ::
        
        #include <mln/morpho/maxtree.hpp>
        #include <mln/core/image/ndimage.hpp>

        mln::image2d<uint8_t> input = ...;

        auto [tree, node_map] = mln::morpho::maxtree(input, mln::c4);


Notes
-----

Complexity
----------
The algorithm is quasi-linear and requires :math:`O(n)` extra-memory space.


References
----------
