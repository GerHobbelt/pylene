Local extrema labelization
==========================

Include :file:`<mln/labeling/local_extrema.hpp>`

.. cpp:namespace:: mln::labeling

.. cpp:function:: \
      template <class Label_t, class I, class N> \
      image_ch_value_t<I, Label_t> local_minima(I input, Neighborhood nbh, int& nlabel)
      template <class Label_t, class I, class N> \
      image_ch_value_t<I, Label_t> local_maxima(I input, Neighborhood nbh, int& nlabel)
      template <class Label_t, class I, class N, class Compare> \
      image_ch_value_t<I, Label_t> local_minima(I input, Neighborhood nbh, int& nlabel, Compare cmp)



      Labelize the regional minima (1) or maxima (2)  of an image with an
      optional comparaison function ``cmp`` (3). According to [Soi13]_, a
      *regional minimum* `M` of an image `f` at elevation `t` is a connected
      component of pixels with the value `t` whose external boundary pixels have
      a value strictly greater than `t`. Similarly, a *regional maximum* `M` of an
      image `f` at elevation `t` is a connected component of pixels with the value `t`
      whose external boundary pixels have a value strictly less than `t`.



      :tparam Label_t: The type of label (must be :cpp:concept:`Integral` and
                       not boolean)

      :param input: Input image
      :param nbh: Neighborhood considered
      :param nlabel (out): Number of extrema detected
      :param cmp (optional): Comparison function


      :return: A labelized image

      :exception: N/A


Notes
-----

Complexity
----------
The algorithm is quasi-linear and requires :math:`n` extra-memory space.


References
----------

.. [Soi13] Soille, P. (2013). Morphological image analysis: principles and applications. Springer Science & Business Media.
