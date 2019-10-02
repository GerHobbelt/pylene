.. _opening-by-reconstruction:

Opening by reconstruction
=========================

Include :file:`<mln/morpho/opening_by_reconstruction.hpp>`

.. cpp:namespace:: mln::morpho

.. cpp:function:: \
   template <class InputImage1, class InputImage2, class Neighborhood> \
   concrete_t<InputImage1> opening_by_reconstruction(const InputImage1& f, const InputImage2& markers, const Neighborhood& nbh)

   Perform the reconstruction of the image ``markers`` under the constrain
   image ``f``. The markers designate the parts of the image we want to
   retain. In binary, it is equivalent to perform the *conditional*
   dilation of the ``markers`` :math:`X`, by a structuring element 𝑩, using
   the *reference* 𝑓 until reaching stability.

   .. math::
      \delta^{n+1}_{f,\mathcal{B}}(X) = \delta_\mathcal{B}(\delta^{n}_{f,\mathcal{B}}(X)) \cap f

   :param f: Input image 𝑓
   :param markers:  Marker image (must have the same value type of 𝑓)
   :param nbh:  Fundamental structuring element.

   :return: An image whose type is deduced from the input image

   :precondition: :math:`markers < f`

   :exception: N/A


Notes
-----


Complexity
----------



Example 1 : Staff lines reconstruction
--------------------------------------

.. image:: /images/staff_lines.png
           :width: 49%

.. image:: /images/staff_lines_markers.png
           :width: 49%

The markers have been obtained with the :cpp:func:`hit_or_miss`.

* Reconstruction of the objects touching staff lines; with the foundamental SE
  (4-connection). All objects that do not touch the staff lines are removed.

.. literalinclude:: /snippets/staff_lines.cpp
   :lines: 56
   :language: cpp

.. image:: /images/staff_lines.png
           :width: 49%

.. image:: /images/morpho_reconstruction_1.png
           :width: 49%

* Reconstruction of the lines only; with an horizontal SE `x-o-x`.


.. literalinclude:: /snippets/staff_lines.cpp
   :lines: 59
   :language: cpp

.. image:: /images/staff_lines.png
           :width: 49%

.. image:: /images/morpho_reconstruction_2.png
           :width: 49%

