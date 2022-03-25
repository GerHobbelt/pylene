Watershed
=========

Include :file:`<mln/morpho/watershed.hpp>`

.. cpp:namespace:: mln::morpho


.. cpp:function::  template <class Label_t, class I, class N> \
                   image_ch_value_t<I, Label_t> watershed(const Image<I>& ima, const Neighborhood<N>& nbh, int& nlabel)

   Watershed by immersion as defined in [BM92]_. The catchment basins are
   labeled from 1 to n, and the special label 0 is used for watershed lines.

   :tparam Label_t: The type of label (must be *signed* :cpp:concept:`Integral`)

   :param input: Input image
   :param nbh: Neighborhood considered
   :param nlabel (out): Number of catchment basins

   :return: A labelized image

   :exception: N/A

.. cpp:function:: template <class Label_t, class I, class N, class S> \
                 image_ch_value_t<I, Label_t> watershed_from_markers(I&& input, N&& nbh, S&& seeds, int& nlabel)

   Watershed by immersion as defined in [BM92]_ with given markers. A marker is **one pixel** on the seed image which is greater than 0.

   :tparam Label_t: The type of label (must be *signed* :cpp:concept:`Integral`)
    and the maximum value of this type must be higher or equal to the maximum
    value of the markers image value type.

   :param input: Input image
   :param nbh: The considered neighborhood
   :param seeds: An image with markers
   :param nlabel (out): The number of catchment basins

   :return: A labelized image

Notes
-----

Complexity
----------
The algorithm is quasi-linear and requires :math:`n` extra-memory space.


References
----------

.. [BM92] Beucher, S., & Meyer, F. (1992). The morphological approach to
          segmentation: the watershed transformation. Optical Engineering-New York-Marcel
          Dekker Incorporated-, 34, 433-433.



Example 1: Cell segmentation
----------------------------

1. The distance transform is performed. Maxima correspond to cell centers.
   A :ref:`dilation <dilation>` by a small disc removes the non-meaningfull maxima.
2. Invertion of the distance image so that maxima become minima
3. Watershed segmentation
4. Input labelization w.r.t in segmentation labels

.. literalinclude:: /snippets/blobs_watershed.cpp
   :start-after: BEGIN
   :end-before: END

.. list-table::

   * - .. image:: /images/blobs_binary.png
          :width: 100%

     - .. image:: /images/blobs_distance_transform.png
          :width: 100%

   * - Input image (note that blobs may touch)
     - Distance transform and dilation (after heat LUT)

.. figure:: /images/blobs_segmentation.png
   :width: 100%
   :align: center

   Segmented blobs and watershed lines (labels displayed in false colors).

Example 2: Watershed from markers
---------------------------------

This example demonstrates how to use the watershed based on input markers.

.. code-block::

   // (1) Get input image and markers image
   mln::image2d<std::uint8_t> input = ...;
   mln::image2d<std::uint16_t> seeds = ...;

   // (2) Compute the Beucher gradient
   auto grad = mln::morpho::gradient(input, mln::se::disc(3));

   // (3) Compute the watershed
    auto out = mln::morpho::watershed_from_markers<std::int32_t>(grad, mln::c8, seeds, nlbl); 

.. list-table::

   * - .. image:: /images/marker_on_gradient.png
          :width: 100%

     - .. image:: /images/lena_ws_marker.png
          :width: 100%

   * - Input image (with markers in red)
     - Segmented image with watershed lines (labels are displayed in false colors).