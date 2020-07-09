Closing
=======

Include :file:`<mln/morpho/structural/closing.hpp>`

.. cpp:namespace:: mln::morpho

.. cpp:function:: \
    Image{I} image_concrete_t<I> closing(I image, StructuringElement se)
    Image{I} image_concrete_t<I> closing(I image, StructuringElement se, BorderManager bm)
    void closing(Image image, StructuringElement se, OutputImage out)
    void closing(Image image, StructuringElement se, BorderManager bm, OutputImage out)

      Closing by a structuring element.

      Given a structuring element 𝐵, the closing :math:`\gamma(f)` of the input
      image 𝑓 is defined as:

      .. math::
         \gamma(f) = \varepsilon_\mathcal{B}(\delta_\mathcal{B}(f))

      * An optional border management may be used to manage border side-effects.
        Only *fill* and *user* are currently supported.

      * If the optional ``output`` image is provided, it must be wide enough to store
        the result (the function does not perform any resizing).
      
      :param ima: Input image 𝑓
      :param se:  Structuring element 𝐵
      :param bm (optional): Border management policy
      :param output (optional): Output image

      :return:
         * (1,2) An image whose type is deduced from the input image
         * (3,4) Nothing (the output image is passed as an argument)

      :exception: N/A


Notes
-----


Complexity
----------



Example 1 : Closing by a square on a gray-level image
------------------------------------------------------

.. code-block:: cpp

   #include <mln/morpho/closing.hpp>
   #include <mln/core/se/rect2d.hpp>

   // Define a square SE of size 21x21
   auto input = ...;
   auto rect = mln::se::rect2d(21,21);
   auto output = mln::morpho::closing(input, rect);


.. image:: /images/lena_gray.jpg
           :width: 49%

.. image:: /images/morpho_closing_1.png
           :width: 49%
