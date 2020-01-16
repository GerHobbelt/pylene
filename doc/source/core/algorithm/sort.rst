Sort_points, sort_indexes
=========================

Include :file:`<mln/core/algorithm/sort.hpp>`

.. cpp:namespace:: mln

#. .. cpp:function:: void sort_points(InputImage ima, OutputRange&& rng)
                     void sort_points(InputImage ima, OutputRange&& rng, Compare cmp)
#. .. cpp:function:: std::vector<image_point_t<InputImage>> sort_points(InputImage ima, OutputRange&& rng)
                     std::vector<image_point_t<InputImage>> sort_points(InputImage ima, OutputRange&& rng, Compare cmp)
#. .. cpp:function:: void sort_indexes(InputImage ima, OutputRange&& rng)
                     void sort_indexes(InputImage ima, OutputRange&& rng, Compare cmp)
#. .. cpp:function:: std::vector<image_point_t<InputImage>> sort_indexes(InputImage ima, OutputRange&& rng)
                     std::vector<image_point_t<InputImage>> sort_indexes(InputImage ima, OutputRange&& rng, Compare cmp)


    Sort the points (versions 1-2) or the indexes (versions 3-4) of an image w.r.t their values.

    :param ima: The source image
    :param rng: (versions 1,3) The destination range where points or indexes are going to be stored.
    :param cmp: (optional) The comparison function between values
    :tparam InputImage: A model of :cpp:concept:`InputImage`
    :tparam OutputRange: A model of :cpp:concept:`OutputRange`
    :tparam Compare: A model of :cpp:concept:`Compare`  
    :return: (versions 2-4) A `std::vector` with the points or the indexes sorted. 

    

Examples
--------

#. Sort the points by increasing level::

    mln::image2d<mln::uint8> f = ...;
    std::vector<mln::point2d> pts = sort_points(f);

#. Sort the image indexes with a lexicographical comparison (a preorder) on red/blue channels::

    mln::image2d<mln::rgb8> f = ...;
    auto comp = [](mln::rb8 x, mln::rgb8 y) { return std::tie(x[0], x[2]) < std::tie(y[0], y[2]); };
    std::vector<mln::point2d> pts = sort_points(f, comp);

Complexity
----------

* In the general case: :math:`O(n \log n)`
* For low-quantized values: :math:`\Theta(n)`
