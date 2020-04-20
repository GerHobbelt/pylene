Morphological Gradients
=======================

Include :file:`<mln/morpho/gradient.hpp>`

.. cpp:namespace:: mln::morpho

..  cpp:function:: \
    Image{I} concrete_t<I> gradient(I f, StructuringElement se)
    Image{I} concrete_t<I> external_gradient(I f, StructuringElement se)
    Image{I} concrete_t<I> internal_gradient(I f, StructuringElement se)

    Compute the morphological gradients. Morphological gradients are
    operators enhancing variations of pixel intensity in a neighborhood
    determined by a structuring element. Three combinations are currently used:

    #. arithmetic difference between the dilation and the erosion; this is basic
       morphological gradient, also called **Beucher** gradient.
        
        :math:`\rho_B = \delta_B - \varepsilon_B`

    #. arithmetic difference between the dilation and the original image; also called **external gradient**:  
        
        :math:`\rho_B^{+} = \delta_B - \mathrm{id}`
    
    #. arithmetic difference between the original image and its erosion; also called the **internal_gradient**:  
    
        :math:`\rho_B^{-} = \mathrm{id} - \varepsilon_B`
    
    If input is not integral, the marginal ordering is considered and the 𝑙₂ of the vector
    difference is returned:

        :math:`\rho_B = \left| \delta_B - \varepsilon_B \right|`
    

   :param f: Input image 𝑓
   :param se:  Elementary structuring element.

   :return: An image whose type is deduced from the input image.

   :precondition:

   :exception: N/A


Example 1 : Gradient by a square on a gray-level image
------------------------------------------------------

.. code-block:: cpp

   #include <mln/morpho/gradient.hpp>
   #include <mln/core/se/rect2d.hpp>

   // Define a square SE of size 7x7
   auto input = ...;
   auto rect = mln::se::rect2d(7,7);
   auto grad1 = mln::morpho::gradient(input, rect);
   auto grad2 = mln::morpho::internal_gradient(input, rect);
   auto grad3 = mln::morpho::external_gradient(input, rect);

.. list-table::

    * - .. figure:: /images/lena_gray.jpg

      - .. figure:: /images/morpho_gradient_1.png

            Beucher (thick) gradient

    * - .. figure:: /images/morpho_int_gradient_1.png

           Internal gradient

      - .. figure:: /images/morpho_ext_gradient_1.png

           External Gradient