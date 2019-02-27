Count If
========

Include :file:`<mln/core/algorithm/count_if.hpp>`

.. cpp:namespace:: mln

.. cpp:function:: std::size_t count_if(InputImage ima, UnaryPredicate p)

    Count the `ima`'s pixels whose value verifies the predicate `p`.

    :param ima: The image to be trasversed
    :param p: The predicate to verify
    :tparam InputImage: A model of :cpp:concept:`InputImage`
    :tparam UnaryPredicate: A model of :cpp:concept:`stl::UnaryPredicate`
    :return: number of pixels whose value verifies `p` in `ima`

    

Examples
--------

#. Count in an image::

    mln::image2d<mln::rgb8> ima = ...;
    std::ptrdiff_t nb_black = mln::count_if(ima, [](mln::rgb8 v){ return v == {0, 0, 0}; });
 
 
Complexity
----------

Linear in the number of pixels.
 