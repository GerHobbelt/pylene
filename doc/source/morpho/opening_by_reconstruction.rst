.. _opening-by-reconstruction:

Opening & Closing by reconstruction
===================================

Include :file:`<mln/morpho/reconstruction.hpp>`

.. cpp:namespace:: mln::morpho

.. cpp:function:: \
   template <Image I1, Image I2> \
   image_concrete_t<I1> opening_by_reconstruction(I1 f, I2 markers, Neighborhood nbh, Compare cmp)
   template <Image I1, Image I2> \
   image_concrete_t<I1> opening_by_reconstruction(I1 f, I2 markers, Neighborhood nbh)
   template <Image I1, Image I2> \
   image_concrete_t<I1> closing_by_reconstruction(I1 f, I2 markers, Neighborhood nbh)

   The opening by reconstruction performs the reconstruction of the image
   ``markers`` under the constrain image ``f``. The markers designate the parts
   of the image we want to retain. In binary, it is equivalent to perform the
   *conditional* dilation of the ``markers`` :math:`X`, by a structuring element
   𝑩, using the *reference* 𝑓 until reaching stability.

   .. math::
      \delta^{n+1}_{f,\mathcal{B}}(X) = \delta_\mathcal{B}(\delta^{n}_{f,\mathcal{B}}(X)) \cap f

   Similarly, the closing by reconstruction reconstructs the *background* instead of the *foreground*.


   :param f: Input image 𝑓
   :param markers:  Marker image (must have the same value type of 𝑓)
   :param nbh:  Elementary structuring element.

   :return: An image whose type is deduced from the input image

   :precondition: :math:`markers \le f` (for (3) :math:`markers \ge f`)

   :exception: N/A


Notes
-----


Complexity
----------



Example 1 : Staff lines reconstruction
--------------------------------------

.. list-table::

   * -   .. figure:: /images/staff_lines.png

            Original image

     -   .. figure:: /images/staff_lines_markers.png

            Markers obtained by the :doc:`hit_or_miss` transform.

Given an original image and some markers obtained with the :doc:`hit_or_miss`
transform. The geodesic reconstruction (with the 4-connection) of the original
image by the markers give the objects touching staff lines. All objects that do
not touch the staff lines are removed.

.. literalinclude:: /snippets/staff_lines.cpp
   :start-after: M3_START
   :end-before: M3_END
   :language: cpp

.. figure:: /images/morpho_reconstruction_1.png
   :figwidth: 49%
   :figclass: align-center

   Geodesic reconstruction from the markers.


If we want to reconstruct only the staff line only, use an horizontal SE `x-o-x`.

.. literalinclude:: /snippets/staff_lines.cpp
   :start-after: M4_START
   :end-before: M4_END
   :language: cpp

.. figure:: /images/morpho_reconstruction_2.png
   :figwidth: 49%
   :figclass: align-center

   Horizontal reconstruction from the markers.

Example 2 : Dense region reconstruction
---------------------------------------

.. list-table::

   *  -  .. figure:: /images/blobs2_binary.png

            a\. Original image


      -  .. figure:: /images/morpho_reconstruction_markers.png

            b\. Markers from the :doc:`rank_filter`

      -  .. figure:: /images/morpho_reconstruction_dilated.png

            c\. Dilated of the original image *a*

.. literalinclude:: /snippets/reconstruction.cpp
   :start-after: M2_START
   :end-before: M2_END
   :language: cpp

Given an original image. We first start with a :doc:`rank_filter` to locate
dense region (regions with much more foreground pixels that background pixels)
that gives us markers. Then a dilation with a small disc allows to connect
objects. The reconstruction of the dilated image with a the markers gives a mask
for the dense region. Finally, we just have to mask the input with the mask to
get the objects in dense regions::

   auto out = mln::clone(rec && input);

.. list-table::

   *  -  .. figure:: /images/morpho_reconstruction_rec.png

            d\. Reconstruction of *c* from the markers *b*

      -  .. figure:: /images/morpho_reconstruction_out.png

            Input (a) restricted to the mask (d)




