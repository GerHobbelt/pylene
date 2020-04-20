Rank filter
===========

Include :file:`<mln/morpho/rank_filter.hpp>`


.. cpp:function:: \
    template <class Ratio> void rank_filter(Image image, StructuringElement se, BorderManager bm, OutputImage out)
    template <class Ratio, Image I> concrete_t<I> rank_filter(I image, StructuringElement se, BorderManager bm)

    
    The rank filter is non-linear filter that assigns the 𝑟-th value in a given structuring element 𝐵.

    .. math::
        g(x) = r(\{ f(y) \in \mathcal{B}_x) \})

    where `r` returns the 𝑟-th value of the set of pixels of the structuring element 𝑩 centered in 𝑥.

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



Example 1 : Rank-filter by a square on a gray-level image
---------------------------------------------------------

.. code-block:: cpp

   #include <mln/morpho/rank_filter.hpp>
   #include <mln/core/se/rect2d.hpp>

   // Define a square SE of size 21x21
   auto input = ...;
   auto rect = mln::se::rect2d(21,21);

   using R = std::ratio<1, 2>; // Get the median value
   auto output = mln::morpho::rank_filter<R>(input, rect, mln::extension::mirror);


.. image:: /images/lena_gray.jpg
           :width: 49%

.. image:: /images/morpho_median_1.png
           :width: 49%
