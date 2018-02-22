Local extrema labelization
==========================

Include :file:`<mln/labeling/local_extrema.hpp>`

.. cpp:namespace:: mln::labeling

#. .. cpp:function:: \
      template <class Label_t, class I, class N> \
      ch_value_t<I, Label_t> local_minima(const Image<I>& input, const Neighborhood<N>& nbh, int& nlabel)

#. .. cpp:function:: \
      template <class Label_t, class I, class N> \
      ch_value_t<I, Label_t> local_maxima(const Image<I>& input, const Neighborhood<N>& nbh, int& nlabel)

#. .. cpp:function:: \
      template <class Label_t, class I, class N, class Compare> \
      ch_value_t<I, Label_t> local_minima(const Image<I>& input, const Neighborhood<N>& nbh, int& nlabel, Compare cmp)

      Labelize the local minima (1) or maxima (2) of an image with an optional
      comparaison function ``cmp`` (3).

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


