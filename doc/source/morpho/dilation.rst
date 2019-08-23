.. _dilation:

Dilation
========

Include :file:`<mln/morpho/dilation.hpp>`

.. cpp:namespace:: mln::morpho


.. cpp:function:: \
      Image{I} concrete_t<I> dilation(I image, StructuringElement se)
      Image{I} concrete_t<I> dilation(I image, StructuringElement se, BorderManager bm)
      void dilation(Image image, StructuringElement se, OutputImage out)
      void dilation(Image image, StructuringElement se, BorderManager bm, OutputImage out)



      Dilation by a structuring element.

      Given a structuring element 𝐵, the dilation :math:`\delta(f)` of the input
      image 𝑓 is defined as:

      .. math::
         \delta(f)(x) = \bigvee \{ \, f(y), y \in B_x \, \}

      * An optional border management may be used to manage border side-effects.
        Only *fill* and *user* are currently supported.

      * If the optional ``output`` image is provided, it must be wide enough to store
        the result (the function does not perform any resizing).

      :param ima: Input image 𝑓
      :param se:  Structuring element 𝐵
      :param bm (optional): Border manager
      :param output (optional): Output image

      :return:
         * (1,2) An image whose type is deduced from the input image
         * (3,4) Nothing (the output image is passed as an argument)

      :exception: N/A


Notes
-----


Complexity
----------



Example 1 : Dilation by a square on a gray-level image
------------------------------------------------------

.. code-block:: cpp

   #include <mln/morpho/dilation.hpp>
   #include <mln/core/wind2d.hpp>

   // Define a square SE of size 21x21
   auto input = ...;
   auto rect = mln::se::rect2d(21,21);
   auto output = mln::morpho::dilation(input, rect);


.. image:: /images/lena_gray.jpg
           :width: 49%

.. image:: /images/morpho_dilation_1.png
           :width: 49%
