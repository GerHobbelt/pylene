Count If
========

Include :file:`<mln/core/algorithm/count.hpp>`

.. cpp:namespace:: mln

.. cpp:function:: std::size_t count(InputImage ima, const Value& v)

    Count the `ima`'s pixels whose value is `v`.

    :param ima: The image to be trasversed
    :param v: The value to be counted
    :tparam InputImage: A model of :cpp:concept:`InputImage`
    :tparam Value: A model of :cpp:concept:`Value`
    :return: number of `v` in `ima`

    

Examples
--------

#. Count in an image::

    mln::image2d<mln::rgb8> ima = ...;
    mln::rgb8 v = ...;
    std::ptrdiff_t nb_v = mln::count(ima, v);
 
 
Complexity
----------

Linear in the number of pixels.
 