Extinction transform
====================

Include :file:`<mln/morpho/extinction_transform.hpp>`

.. cpp:namespace:: mln::morpho

.. cpp:function:: \
    Image{I} image_concrete_t<I> minima_extinction_transform(I ima, Neighborhood nbh)
    Image{I} image_concrete_t<I> maxima_extinction_transform(I ima, Neighborhood nbh)
 
    The notion of *extinction* of a local extremum is based on the h-extrema (see. :doc:`dynamic_filter`).
    The extinction value of a bassin corresponds its *dynamic* when it merges with another basin.
    By definition, the lowest image minima / highest image maxima, have an *extinction* equal to the difference
    between the highest and lowest image grey scale values.
 
    Let Mₜ be a regional maximum at level *t* of an image *f*, and 𝓟 any path
    linking a pixel *p* of Mₜ to a pixel *q* of a regional maximum Mₜ' higher
    than Mₜ. In [Soi13]_, its *extinction* of Mₜ is defined as:

    .. math::
      
        \min_{\mathcal{P} = \{p, \cdots, q\}} \left\{ \max_{x \in \mathcal{P}} (f(p) - f(x)) \right\}

    .. note::
        Pixels that do not belong to a local extremum are set to 0.

    .. figure:: /figures/morpho/extinction.svg

        Extinction transform of a 1D signal. Red arrows show the extinction value of each minima.

    :param f: Input image 𝑓
    :param nbh:  Elementary structuring element.
 
    :return: An image whose type is deduced from the input image
 
    :exception: N/A


Notes
-----


Complexity
----------

References
----------

.. [Soi13] Soille, P. (2013). Morphological image analysis: principles and applications. Springer Science & Business Media.