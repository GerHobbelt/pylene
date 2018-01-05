Opening
=======

#. .. cpp:function:: \
      template <class InputImage, class StructuringElement> \
      concrete_t<InputImage> opening(const InputImage& ima, const StructuringElement& se)

#. .. cpp:function:: \
      template <class InputImage, class StructuringElement, class Compare> \
      concrete_t<InputImage> opening(const InputImage& ima, const StructuringElement& se, Compare cmp)

#. .. cpp:function:: \
      template <class InputImage, class StructuringElement, class OutputImage, class Compare> \
      void opening(const InputImage& ima, const StructuringElement& se, Compare cmp, OutputImage& output)

      Opening by a structuring element.

      Given a structuring element 𝐵, the opening :math:`\gamma(f)` of the input
      image 𝑓 is defined as:

      .. math::
         \gamma(f) = \delta_\mathcal{B}(\varepsilon_\mathcal{B}(f))

      * (2,3) If a optional \p cmp function is provided, the algorithm will internally do
        an unqualified call to ``inf(x, y,cmp)``.The default is the product-order so
        that it works for vectorial type as well.

      * (3) If the optional ``output`` image is provided, it must be wide enough to store
        the results (the function does not perform any resizing).

      :param ima: Input image 𝑓
      :param se:  Structuring element 𝐵
      :param cmp (optional): Comparison function
      :param output (optional): Output image

      :return:
         * (1,2) An image whose type is deduced from the input image
         * (3\) Nothing (the output image is passed as an argument)

      :exception: N/A


Notes
-----


Complexity
----------



Example 1 : Opening by a square on a gray-level image
------------------------------------------------------

.. code-block:: cpp

   #include <mln/morpho/structural/opening.hpp>
   #include <mln/core/wind2d.hpp>

   // Define a square SE of size 21x21
   auto input = ...;
   auto rect = mln::make_rectangle2d(21,21);
   auto output = mln::morpho::structural::opening(input, rect);


.. image:: /images/lena_gray.jpg
           :width: 49%

.. image:: /images/morpho_opening_1.png
           :width: 49%
