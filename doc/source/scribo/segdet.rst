Line Detection
======================

* Include :file:`<mln/contrib/segdet/segdet.hpp>`

.. cpp:namespace:: mln::contrib::segdet

.. cpp:function:: segdet_output detect_line(mln::image2d<std::uint8_t> input, int min_len, const scribo::SegDetParams& params);

    Detects the lines that are presents in the given image and return all the points matched to a line.

    :param input: The input image, it must be a grey-level image
    :param min_len: The minimum length (in pixels) of segments that have to be detected
    :param params: The Parameters struct giving the parameters of the method.
    :param dist: The function weighting the edges between two pixels.
    :return: A segdet_output struct containing vectors that have the information on points that have been detected.

Definition
----------

Retrieving straight or slightly curved lines out of document images can be a essentiel step of document analysis.
This method retrieves such lines by iteratively predicting the position of spans of pixels in the input image columns.
In order to do so, it makes use of Kalman filters to integrate the observed measurements and determine what is and what is not a segment.

Notes
-----

Complexity
----------

References
----------

.. [Lep95] Leplumey, Ivan, Jean Camillerapp, and Charles Queguiner. "Kalman filter contributions towards document segmentation." Proceedings of 3rd International Conference on Document Analysis and Recognition. Vol. 2. IEEE, 1995.