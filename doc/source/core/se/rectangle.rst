Rectangle
=========

.. doxygenstruct:: mln::experimental::se::rect2d
   :members:


.. rubric:: Performance

The rectangle is decomposable in two lines, so that the speed of a dilation (or
erosion) by a rectangle does not depend on the size of the rectangle.


Example 1. Dilation by rectangle
--------------------------------

.. code-block:: pylenecpp

   #include <mln/core/se/rect2d.hpp>

   mln::se::rect2d (21, 21);
   auto output = mln::morpho::structural::dilate(input, rect);


.. image:: /images/lena_gray.jpg
           :width: 49%

.. image:: /images/morpho_dilation_1.png
           :width: 49%
