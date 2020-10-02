Using algorithms
================


Using the library is straighforward. Many ready-to-use algorithm just takes images as input and returns an
image. Consider the algorithm :cpp:func:`mln::morpho::dilation`. In its simplest form, this algorithm
takes an image, a structuring element and returns an image, ``dilation(in, se) -> out``::

  #include <mln/morpho/dilation.hpp>
  #include <mln/core/se/disc.hpp>
  #include <mln/io/imread.hpp>
  #include <mln/io/imsave.hpp>

  mln::image2d<uint8_t> input;
  mln::io::imread("input.tiff", input);

  auto output = mln::morpho::dilation(input, mln::se::disc(5));
  mln::io::imsave(output, "out.tiff");




Output image convention
-----------------------


Most algorithms have two versions:

#. ``out = algo(in)`` where the output image is allocated by the algorithm and returned
#. ``algo(in, out)`` where the user has to provide the output image

Usually, the first version allocates and forward to the second version. The previous snippet of code is equivalent to::

  mln::image2d<uint8_t> input;
  mln::io::imread("input.tiff", input);

  mln::image2d<uint8_t> output(input.width(), input.height);
  mln::morpho::dilation(input, mln::se::disc(5), output);
  mln::io::imsave(output, "out.tiff");




