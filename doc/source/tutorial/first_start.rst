Creating and loading images
===========================

.. cpp:namespace:: mln

Regular 2D images from scratch
------------------------------

:cpp:any:`image2d\<T> <image2d>` is the most-used image type representing images defined on a 2D grid. It is a template
class, where the parameter `T` can be replaced by the type of value to store. For instance, if it is a grayscale image
with values encoded on 8-bits unisgned integers, `T` will be `uint8_t`. For RGB-8 images, `T` is ``mln::rgb8``. Any
*regular* (foundamental) type can be used, fixed-size scalar and floating-point types are already defined in C++ in the
`<cstdint> <https://en.cppreference.com/w/cpp/header/cstdint>`_ header. The template parameter `T` is that we call the
*value type* (every image has a *value type*, be it static or dynamic)


An image might be created from a set of values (this is generally used only for testing an algorithm). For instance, in
the following code, a 2 × 3 2D image is created with values from 1 to 6::

  #include <mln/core/image/ndimage.hpp>
  mln::image2d<uint8_t> f = {{1,2,3}, {4,5,6}};

It is generally more commom to create a 2D image from its size, using its width and its height to reserve space. Note
that values are (generally) undefined::

  // Create a 800x600 2D images
  mln::image2d<uint8_t> f(800, 600);

A last parameter allows to customize the initialization of an image. It can be used to initialize an image with a custom
value, e.g. the grayscale value 128::

  // Create a 800x600 2D images
  mln::image2d<uint8_t> f(800, 600, {.init_value = uint8_t(128)})



Finally a 2D image may not start at location `(0,0)`. For instance when want to work a sub-regions of an image, the
location of the origin has moved. Every image is a tied to a *domain*, which is a :cpp:any:`box2d` for 2D images.
One can initialize an image from a domain of a pair *(width, height)*::

  // A roi (x=50,y=50,w=100,h=100)
  mln::box2d roi = {{50,50}, {150,150}};
  mln::image2d<uint8_t> f(roi);

Regular ND images
-----------------

The class :cpp:any:`image2d` is generalized in any dimension (:cpp:any:`image1d`, :cpp:any:`image3d` and so
on). Actually, they are aliases over the type (:cpp:any:`ndimage\<T, N> <__ndbuffer_image>` where the
dimension is fixed).

Thus you can create 3D-images from a initializer-lists::

  #include <mln/core/image/ndimage.hpp>

  // A 2x2x3 3D-image
  mln::image2d<uint8_t> f = {
    {{1,2,3}, {4,5,6}},
    {{7,8,9}, {10,11,12}}
  };


From a triplet (width,height,depth)::

  // Create a 800x600 2D images
  mln::image2d<uint8_t> f(128, 128, 128);

From a :cpp:any:`box3d` domain::

  // A roi (x=50,y=50,z=50,w=100,h=100,d=100)
  mln::box2d roi = {{50,50,50}, {150,150,150}};
  mln::image3d<uint8_t> f(roi);


Loading and saving an image
---------------------------

An image can be loaded from a file. It actually relies on the
`freeimage <http://freeimage.sourceforge.net/>`_ library to
handle most file formats::

  #include <mln/core/image/ndimage.hpp>
  #include <mln/io/imread.hpp>

  {
    mln::image2d<uint8_t> f;
    mln::io::imread("input.pgm", f);
  }
  {
    mln::image2d<uint8_t> f = { {1,2,3}, {4,5,6} };
    mln::io::imsave(f, "out.pgm");
  }


Images from existing data
-------------------------

In your application, would will mostly want to use algorithms from many libraries that do not share the image types.  As
soon as an image is encoded as C raw-buffer of data, you will be able to exchange images from/to pylene using
:cpp:any:`__ndbuffer_image::from_buffer`.

::

   uint8_t data[] = {255, 0, 0, 255};
   int dims[] = {2, 2};
   auto input = mln::image2d<uint8_t>::from_buffer(data, dims);

And you can access the Pylene internal buffer with :cpp:func:`buffer <mln::__ndbuffer_image::buffer>`


























