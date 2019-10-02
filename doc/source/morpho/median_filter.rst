Median filter
=============

Include :file:`<mln/morpho/median_filter.hpp>`


.. cpp:function:: \
    void median_filter(Image image, StructuringElement se, BorderManager bm, OutputImage out)
    Image{I} concrete_t<I> median_filter(I image, StructuringElement se, BorderManager bm)

    
    The median filter is non-linear filter that assigns the median value in a given structuring element 𝐵.

    .. math::
        g(x) = med(\{ f(y) \in \mathcal{B}_x) \})

    where `med` returns the median value of the set of pixels in the structuring element 𝑩 centered in 𝑥.

    * A border management may be used to manage border side-effects.

    * If the optional ``output`` image is provided, it must be wide enough to store
      the result (the function does not perform any resizing).

    :param ima: Input image 𝑓
    :param se:  Structuring element 𝐵
    :param bm: Border manager policy
    :param output (optional): Output image

    :return:
       * (1) Nothing (the output image is passed as an argument)
       * (2) An image whose type is deduced from the input image
    
    :exception: N/A


Notes
-----


Complexity
----------



Example 1 : Median-filter by a square on a gray-level image
---------------------------------------------------------

.. code-block:: cpp

   #include <mln/morpho/median_filter.hpp>
   #include <mln/core/se/rect2d.hpp>

   // Define a square SE of size 21x21
   auto input = ...;
   auto rect = mln::se::rect2d(21,21);

   auto output = mln::morpho::median_filter(input, rect, mln::extension::mirror);


.. image:: /images/lena_gray.jpg
           :width: 49%

.. image:: /images/morpho_median_1.png
           :width: 49%
