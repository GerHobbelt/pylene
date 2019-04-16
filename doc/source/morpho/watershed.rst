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



