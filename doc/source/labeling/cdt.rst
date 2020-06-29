Chamfer Distance Transform
==========================

Include :file:`<mln/labeling/chamfer_distance_transform.hpp>`

.. cpp:namespace:: mln::labeling




.. cpp:function:: template <typename DistanceType = int, Image I, WeightedNeighborhood N> \
    image_ch_value_t<InputImage, DistanceType> \
    chamfer_distance_transform(I input, N nbh, bool background_is_object = false)

    Perform the distance transform [1]_ using the given Neighborhood and its weights. Each foreground pixel
    gets the distance to the closest background pixel.

    Note that the sum saturates to the maximum value that can be taken by `DistanceType`.

    :tparam DistanceType: The type used for the accumulation
    :param input: Input image
    :param nbh: Neighborhood (weighted)
    :param background_is_object (optional): Whether to consider the domain "outside" is the foreground (object) or the background.
    :return: A distance image
    :exception: N/A


Notes
-----

Complexity
----------

Linear in pixels (2 passes).


Example
-------

Example with the chessboard distance::

    #include <mln/labeling/chamfer_distance_transform.hpp>
    #include <mln/core/neighborhood/c8.hpp>

    auto out = mln::labeling::chamfer_distance_transform<int16_t>(input, mln::c8, true);

Example with the 5-7-11 weights::

    #include <mln/labeling/chamfer_distance_transform.hpp>
    #include <mln/core/se/mask2d.hpp>

    auto weights = mln::se::wmask2d({{+0, 11, +0, 11, +0}, //
                                     {11, +7, +5, +7, 11},
                                     {+0, +5, +0, +5, +0},
                                     {11, +7, +5, +7, 11},
                                     {+0, 11, +0, 11, +0}});

    auto out = mln::labeling::chamfer_distance_transform<int16_t>(input, weights, true);


.. list-table::

    * -   .. figure:: /images/F.png

            Original image

      -   .. figure:: /images/F-4.png

            CDT with :cpp:ref:`mln::c4` (Manhattan distance)

      -   .. figure:: /images/F-8.png

            CDT with :cpp:ref:`mln::c8` (Chessboard distance)

    * -   .. figure:: /images/F-2-3.png

            CDT with 2-3 :cpp:ref:`mln::wmask2d` (2-3 distance)

      -   .. figure:: /images/F-5-7-11.png

            CDT with 5-7-11 :cpp:ref:`mln::wmask2d` (5-7-11 distance)


References
----------

.. [1] Borgefors, G. (1986). Distance transformations in digital images. Computer vision, graphics, and image processing, 34(3), 344-371.
