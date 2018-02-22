Closing by reconstruction
=========================

Include :file:`<mln/morpho/closing_by_reconstruction.hpp>`

.. cpp:namespace:: mln::morpho


.. cpp:function:: \
   template <class InputImage1, class InputImage2, class Neighborhood> \
   concrete_t<InputImage1> closing_by_reconstruction(const InputImage1& f, const InputImage2& markers, const Neighborhood& nbh)

.. cpp:function:: \
   template <class InputImage1, class InputImage2, class Neighborhood, class Compare> \
   concrete_t<InputImage1> closing_by_reconstruction(const InputImage1& f, const InputImage2& markers, const Neighborhood& nbh, Compare cmp)

   Perform the reconstruction of the image ``markers`` over the constrain
   image ``f``. Contrary to :ref:`opening-by-reconstruction`, the *background* is
   reconstructed instead of the *foreground*.


   :param f: Input image 𝑓
   :param markers:  Marker image (must have the same value type of 𝑓)
   :param nbh:  Fundamental structuring element.
   :param cmp (optional): Comparaison function defining a strict weak ordering of values.

   :return: An image whose type is deduced from the input image

   :precondition: :math:`markers > f`

   :exception: N/A


Notes
-----


Complexity
----------

