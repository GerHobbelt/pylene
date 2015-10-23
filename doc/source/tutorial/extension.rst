Image border and extension
==========================

The library provides extension mechanisms for the image domain. In the
library, both the concept of border and of extension can be
encountered. These concepts are useful in many algorithms and can
avoid costly tests while working with sites located on image edges.

Image border
------------

A border is a finite extension provided to a basic image type, such as
:cpp:class:`mln::image2d`. By default, every image (supporting
borders) is created with a border::

  #include <mln/core/image/image2d.hpp>
  #include <mln/core/extension/fill.hpp>
  #include <mln/io/imprint.hpp>

  mln::image2d<int> f = { {1,2,3}, {4,5,6} };
  mln::extension::fill(f, 7);
  mln::io::imprint_with_border(f);


.. code-block:: none

   7 7 7 7 7 7 7 7 7 
   7 7 7 7 7 7 7 7 7 
   7 7 7 7 7 7 7 7 7 
   7 7 7 1 2 3 7 7 7 
   7 7 7 4 5 6 7 7 7 
   7 7 7 7 7 7 7 7 7 
   7 7 7 7 7 7 7 7 7 
   7 7 7 7 7 7 7 7 7


It is important to note that to display the border in the ouput, we
use a special debug function, debug::println_with_border. Indeed, the
border and the extension are considered as part of an image only in
the algorithms. They are ignored while saving or printing an image.

Some operations can be performed on the border. The functions are
located in the namespace ``mln::extension``.


+------------------------+----------------------------------------------------------------------+
| ``need_adjust(f, se)`` | True if the extension of `f` is wide enough to hold the structuring  |
|                        | element `se` .                                                       |
+------------------------+----------------------------------------------------------------------+
| ``fill(f, v)``         | Fill the extension of `f` with `v`                                   |
+------------------------+----------------------------------------------------------------------+
| ``mirror(f)``          | Fill the extension by mirroring `f`                                  |
+------------------------+----------------------------------------------------------------------+
| ``periodize(f)``       | Fill the extension by periodizing `f`                                |
+------------------------+----------------------------------------------------------------------+
| ``duplicate(f)``       | Fill the extension of `f` with the nearest neighbor                  |
+------------------------+----------------------------------------------------------------------+

.. note::

   In |milena|, there are routines able to resize the border. Resizing
   the border used to silently duplicate the image. In |pylene|,
   resizing the border is not allowed. Instead, a performance warning
   is issued to the user so that he can consider creating the image
   with a larger border to improve efficiency.


Different type of extension
---------------------------

There are different type of extension:

Border extension
  An extension with memory allocated to it
Value extension
  An constant extension with a fixed value
Image extension
  An extension defined by another image
Function extension
  An extension defined by a function

One will be able to add the three last extensions with:


+-----------------------------------------------+----------------------------------------------------+
| ``auto g = add_value_extension(f, v)``        | Return an image with a value extension set to `v`  |
|                                               |                                                    |
+-----------------------------------------------+----------------------------------------------------+
| ``auto g = add_function_extension(f, fun)``   | Return an image with the extension defined by the  |
|                                               | function `fun` (Not implemented)                   |
+-----------------------------------------------+----------------------------------------------------+
| ``auto g = add_image_extension(f, f₂)``       | Return an image whose extension is defined by the  |
|                                               | image `f₂` (Not implemented)                       |
+-----------------------------------------------+----------------------------------------------------+
