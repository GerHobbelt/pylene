Waterfall
=========

Include :file:`<mln/morpho/waterfall.hpp>`

.. cpp:namespace:: mln::morpho

.. cpp:function::  template <typename I, typename N> \
                   auto waterfall(I ima, N nbh)

   Implementation of the Waterfall algorithm [B94]_ described in [M15]_. The
   Waterfall is an iterative process starting from a watershed segmentation and
   removing the watershed lines surrounded by watershed lines whose gradient is
   bigger. The resulting of the last iteration is the complete image domain.

   :param ima: The input image
   :param nbh: The neighborhood relationship considered to build the watershed segmentation
   :return: A pair `(tree, node_map)` where `tree` is the hierarchical
    representation of the Waterfall and `node_map` is a mapping from a point of
    the image to a node of the tree. This node map is usually the initial
    watershed segmentation.

References
----------

.. [B94] Beucher, S. (1994). Watershed, hierarchical segmentation and waterfall
    algorithm. In Mathematical morphology and its applications to image
    processing (pp. 69-76). Springer, Dordrecht.

.. [M15] Meyer, F. (2015, May). The waterfall hierarchy on weighted graphs. In
    International Symposium on Mathematical Morphology and Its Applications to
    Signal and Image Processing (pp. 325-336). Springer, Cham.

Example
-------

.. code-block::

    // (1) Get an image
    mln::image2d<std::uint8_t> input = ...;

    // (2) Compute the gradient (here using the morphological gradient)
    auto grad = mln::morpho::gradient(input, mln::se::disc(3));

    // (3) Compute the waterfall hierarchy
    auto [t, nodemap] = mln::morpho::waterfall(grad, mln::c8);

    // (4) Compute the cut at level 4
    t.horizontal_cut(4, nodemap);

.. list-table::

    * - .. image:: /images/lena_gray.jpg
           :width: 100%

      - .. image:: /images/waterfall_nodemap.png
           :width: 100%
    
      - .. image:: /images/waterfall_cut.png
           :width: 100%

    * - Original image
      - Watershed segmentation on the gradient of the original image  
      - Waterfall horizontal cut at level 4