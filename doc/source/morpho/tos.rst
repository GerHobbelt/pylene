Tree of Shapes (ToS)
====================

Include :file:`<mln/morpho/tos.hpp>`

.. cpp:namespace:: mln::morpho


.. cpp:function:: Image{I} auto tos(I f, image_point_t<I> pinf)

    Compute the ToS and returns a pair `(tree, node_map)`. See :doc:`component_tree` for
    more information about the representation of tree. The value_type
    of the image must be an unsigned integral. It only handles regular 2D or 3D domains.
    
    .. note:: Note that the size of the image is twice the original image size.



    :param input: The input image
    :param pinf: Rooting point
    :return: A pair `(tree, node_map)` where *tree* is of type `component_tree<image_value_t<I>>` and
             
    :precondition: ``f.domain().has(pinf)``

    .. rubric:: Requirements

    * ``image_value_t<I>`` is a :cpp:concept:`UnsignedIntegral`
    * ``image_domain_t<I>`` is :cpp:class:`mln::box2d` or :cpp:class:`mln::box3d`

    .. rubric:: Example

    ::
        
        #include <mln/morpho/tos.hpp>
        #include <mln/core/image/ndimage.hpp>

        mln::image2d<uint8_t> input = ...;

        auto [tree, node_map] = mln::morpho::tos(input);


Notes
-----

Complexity
----------
The algorithm is linear and requires :math:`O(n)` extra-memory space.


References
----------
