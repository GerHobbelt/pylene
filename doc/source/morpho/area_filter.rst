Opening and Closing by Area
===========================

Include :file:`<mln/morpho/area_filter.hpp>`

.. cpp:namespace:: mln::morpho

.. cpp:function:: \
    Image{I} concrete_t<I> area_opening(I f, Neighborhood nbh, int area, Compare cmp)
    Image{I} concrete_t<I> area_closing(I f, Neighborhood nbh, int area)


    On binary images, the area connected opening that preserves connected
    components (blobs) of a minimum size. On grayscale images, it extracts
    connected image objects of higher intensity values than the surrounding
    objects. (The area closing is its complementary operator).


   :param f: Input image 𝑓
   :param nbh:  Elementary structuring element.
   :param area: The minimal size of the blobs in order to be preserved

   :return: An image whose type is deduced from the input image

   :exception: N/A


Notes
-----


Complexity
----------


Example: dense objects detection
--------------------------------

.. list-table::

   *  -  .. figure:: /images/blobs2_binary.png

            (a) Original image


      -  .. figure:: /images/morpho_area_filter_dilated.png

            (b) Dilated of the original image (a)

   *  -  .. figure:: /images/morpho_area_filter_dilated.png

            (c) Result of the area opening of (b)


      -  .. figure:: /images/morpho_area_filter_out.png

            (d) Input image masked by (c)


.. literalinclude:: /snippets/area_filter.cpp
   :start-after: M2_START
   :end-before: M2_END
   :language: cpp

Given an original image. A dilation with a small disc allows to connect
objects and we remove small connected components with an area opening. 
Finally, we just have to mask the input with the mask to get the objects in dense regions.

