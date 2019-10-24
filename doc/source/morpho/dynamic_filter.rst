Opening and Closing by Dynamic
==============================

Include :file:`<mln/morpho/area_filter.hpp>`

.. cpp:namespace:: mln::morpho

.. cpp:function:: \
    Image{I} concrete_t<I> dynamic_opening(I f, Neighborhood nbh, int h)
    Image{I} concrete_t<I> dynamic_closing(I f, Neighborhood nbh, int h)


    On grayscale images, the dynamic connected opening removes maxima whose
    *dynamic* is below a given value where the dynamic stands for the difference
    between the lowest and highest value of the component. In [Soi13]_, it
    refers to the notion of h-extrema filtering that suppresses all maxima whose
    depth is lower or equal to a given threshold level *h*. This is achieved by
    performing the reconstruction by dilation of *f* from :math:`f - h`.

    .. figure:: /figures/morpho/hextrema.svg

        Dynamic opening with a depth *h=3*. The blue function is the original image *f* and is light gray is the opening.


    :param f: Input image 𝑓
    :param nbh:  Elementary structuring element.
    :param h: The minimal dynamic to preserve a component
 
    :return: An image whose type is deduced from the input image
 
    :exception: N/A
 

Notes
-----


Complexity
----------



References
----------

.. [Soi13] Soille, P. (2013). Morphological image analysis: principles and applications. Springer Science & Business Media.