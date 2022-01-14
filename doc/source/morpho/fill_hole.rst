Fill holes
==========

Include :file:`<mln/morpho/fill_hole.hpp>`

.. cpp:namespace:: mln::morpho

.. cpp:function:: \
    Image{I} image_concrete_t<I> fill_hole(I f, Neighborhood nbh, image_point_t<I> marker)
    Image{I} void fill_hole(I f, Neighborhood nbh, image_point_t<I> marker, Image out)
 
    The holes of a binary image correspond to the set of its regional minima which
    are not connected to the image border (or a given `marker`). It is defined as the complement
    of the background connected component of the marker.

    .. math::
      
        E \setminus CC(p)


    :param f: Input image 𝑓 of type bool.
    :param nbh:  Elementary structuring element.
    :param marker: Reference point for background
 
    :return: A binary image whose type is deduced from the input image
 
    :exception: N/A


Notes
-----


Complexity
----------

References
----------

.. [Soi13] Soille, P. (2013). Morphological image analysis: principles and applications. Springer Science & Business Media.