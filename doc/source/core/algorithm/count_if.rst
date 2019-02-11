Count If
========

Include :file:`<mln/core/algorithm/count_if.hpp>`

.. cpp:namespace:: mln

.. cpp:function:: std::ptrdiff_t count_if(InputImage ima, std::UnaryPredicate p)

    Count the number of pixel whose value verify the predicate `p` in `ima`.

    :param ima: The image to be trasversed
    :param p: The predicate to verify
    :tparam InputImage: A model of :cpp:concept:`InputImage`
    :tparam std::UnaryPredicate: A model of :cpp:concept:`std::UnaryPredicate`
    :return: number of pixels whose value verifies `p` in `ima`

    

Examples
--------

#. Counts the number of non-black pixel in an image::

    mln::image2d<mln::rgb8> ima = ...;
    mln::rgb8 black{0, 0, 0};
    std::ptrdiff_t nb_black = mln::count_if(ima, [&](mln::rgb8 v){ return v != black; });
 
 
Complexity
----------

Linear in the number of pixels.
 