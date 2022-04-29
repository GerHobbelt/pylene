Waterfall
=========

Example
-------

.. code-block::

    // (1) Get an image
    mln::image2d<std::uint8_t> input = ...;

    // (2) Compute the gradient (here using the morphological gradient)
    auto grad = mln::morpho::gradient(input, mln::se::disc(3));

    // (3) Compute the waterfall hierarchy
    auto [t, nodemap] = mln::morpho::waterfall(grad, mln::c8);

    // (4) Compute the saliency of the waterfall
    auto saliency = mln::morpho::waterfall_saliency(t, nodemap);

    // (5) Compute the horizontal cut at level 4 from the saliency
    auto thresholded_saliency = saliency >= 4;
    int  nlbl;
    auto cut = mln::labeling::blobs<std::uint16_t>(1 - thresholded_saliency, mln::c8, nlbl);

.. list-table::

    * - .. image:: /images/lena_gray.jpg
           :width: 100%

      - .. image:: /images/waterfall_nodemap.png
           :width: 100%

    * - Original image
      - Watershed segmentation on the gradient of the original image

    * - .. image:: /images/waterfall_saliency.png
           :width: 100%
    
      - .. image:: /images/waterfall_cut.png
           :width: 100%

    * - Saliency of the waterfall
      - Waterfall horizontal cut at level 4