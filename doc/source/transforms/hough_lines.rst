Hough line detection
====================

* Include :file:`<mln/transforms/hough_lines.hpp>`

.. cpp:namespace:: mln::transforms


.. cpp:function:: image2d<int> hough_lines(const image2d<bool>& input, std::span<float> angles, image2d<int>* out = nullptr);

    Compute the hough transform a binary image and returns the vote image

    :param input: The input binary image
    :param angles: The angles (in radians) for which to vote (of length W)
    :param out: (optional) The output image of size (W × H) where the vote will be added. It is useful for example if
                you want α and α ± π/2 to vote for the same thing.
    :return: The output image of size (W × H) where H is the length of the diagonal
    :rtype: image2d<int>


.. cpp:function:: std::vector<HoughLine> hough_lines_detect_peaks(const image2d<int>&   acc,                  \
                                                                  std::span<float>    angles,               \
                                                                  float               intensity_reject = 0.5f, \
                                                                  int                 min_sep_distance = 5, \
                                                                  int                 min_sep_angle    = 5, \
                                                                  int                 max_num_peaks        = 10);

    Detect the peaks in a Hough vote image

    :param acc: The vote image of size (W x H)
    :param angles: The angles used to generate the vote image (of length W)
    :param instensity_reject: The percentage of the global max peak value to consider the local max as a peak
    :param min_sep_distance: Size of the Non-Maximum-Suppression (NMS) window (units of radius in pixels)
    :param min_sep_angle: Size of the Non-Maximum-Suppression (NMS) window (units of angles)
    :param max_num_peaks: The maximum number of peaks returned
    :return: The peaks of the vote image ordered by vote
    :rtype: std::vector<HoughLine>

.. cpp:function:: std::vector<HoughLineAngle> hough_lines_detect_peak_angles(const image2d<int>& acc, \
                                                                            std::span<float>    angles, \
                                                                            float               intensity_reject = 0.5f, \
                                                                            int                 min_sep_distance = 5, \
                                                                            int                 min_sep_angle    = 5);

    Detect the main angles in a Hough vote image

    :param acc: The vote image of size (W x H)
    :param angles: The angles used to generate the vote image (of length W)
    :param instensity_reject: The percentage of the global max peak value to consider the local max as a peak
    :param min_sep_distance: Size of the Non-Maximum-Suppression (NMS) window (units of radius in pixels)
    :param min_sep_angle: Size of the Non-Maximum-Suppression (NMS) window (units of angles)
    :return: The main directions in the image
    :rtype: std::vector<HoughLineAngle>

Usage
-----

Retrieve straight lines in images



.. literalinclude:: /snippets/hough_lines.cpp
   :start-after: BEGIN
   :end-before: END

(Full code: :download:`/snippets/hough_lines.cpp`)

.. list-table::

   * - .. image:: /images/staff_lines.png
          :width: 100%

     - .. image:: /images/staff_hough_lines.png
          :width: 100%

   * - Input image
     - Hough lines detected (super-imposed)

