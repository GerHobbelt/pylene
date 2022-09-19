Line Detection using Multiple Object Tracking
=============================================

* Include :file:`<scribo/segdet.hpp>`

.. cpp:namespace:: scribo

.. cpp:function:: std::tuple<mln::image2d<std::uint16_t>, std::vector<LSuperposition>> detect_line_pixel(const mln::image2d<std::uint8_t>& input, int min_len, const SegDetParams& params);
    
.. cpp:function:: std::vector<VSegment> detect_line_vector(const mln::image2d<std::uint8_t>& input, int min_len, const SegDetParams& params);

.. cpp:function:: std::tuple<mln::image2d<std::uint16_t>, std::vector<LSuperposition>, std::vector<VSegment>> detect_line_full(const mln::image2d<std::uint8_t>& input, int min_len, const SegDetParams& params);

    Compute a line detection with a complete pipeline using MOT in a greyscale image.

    :param input: The input greyscale image
    :param min_len: The minimum length (in pixels) of segments that have to be detected
    :param params: The Parameters struct giving the parameters of the method.

Input
-----

.. doxygenenum:: scribo::SEGDET_PREPROCESS_ENUM
.. doxygenenum:: scribo::SEGDET_PROCESS_TRACKING_ENUM
.. doxygenenum:: scribo::SEGDET_PROCESS_EXTRACTION_ENUM
.. doxygenenum:: scribo::SEGDET_PROCESS_TRAVERSAL_MODE_ENUM

.. doxygenstruct:: scribo::SegDetParams
   :members:

Outputs
-------

.. doxygenstruct:: scribo::VSegment
   :members:
   
.. doxygenstruct:: scribo::LSuperposition
   :members:

Definition
----------

Retrieving straight or slightly curved lines out of document images can be a essentiel step of document analysis.
This method retrieves such lines by iteratively predicting the position of spans of pixels in the input image columns.
In order to do so, it makes use of Kalman filters to integrate the observed measurements and determine what is and what is not a segment.

Usage
-----

Retrieve lines in document images

.. literalinclude:: /snippets/segdet.cpp
   :start-after: BEGIN 1
   :end-before: END 1

Build vectorial output image

.. literalinclude:: /snippets/segdet.cpp
   :start-after: BEGIN 2
   :end-before: END 2

Build pixel output image

.. literalinclude:: /snippets/segdet.cpp
   :start-after: BEGIN 3
   :end-before: END 3

Display superposition information

.. literalinclude:: /snippets/segdet.cpp
   :start-after: BEGIN 4
   :end-before: END 4

(Full code: :download:`/snippets/segdet.cpp`)

.. list-table::

   * - .. image:: /images/staff_lines.png
          :width: 100%
   
     - .. image:: /images/staff_scribo_lines_vector.png
          :width: 100%

     - .. image:: /images/staff_scribo_lines_pixel.png
          :width: 100%

   * - Input image
     - Scribo lines detected vector (super-imposed)
     - Scribo lines detected pixel (super-imposed)

References
----------

.. [Lep95] Leplumey, Ivan, Jean Camillerapp, and Charles Queguiner. "Kalman filter contributions towards document segmentation." Proceedings of 3rd International Conference on Document Analysis and Recognition. Vol. 2. IEEE, 1995.