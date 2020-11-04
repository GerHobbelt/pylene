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


Parallel algorithms
-------------------

Pylene supports multithreaded parallelism in some of its algorithms namely for the following pointwise algorithms.
The parallel versions of the algorithms are available in the namespace ``parallel::``.
Using the previous code snippet as an example we would get::

  #include <mln/morpho/dilation.hpp>
  #include <mln/core/se/disc.hpp>
  #include <mln/io/imread.hpp>
  #include <mln/io/imsave.hpp>

  mln::image2d<uint8_t> input;
  mln::io::imread("input.tiff", input);

  auto output = mln::morpho::parallel::dilation(input, mln::se::disc(5));
  mln::io::imsave(output, "out.tiff");

You can also get better performances by specifying appropriate tile sizes depending on the size of the input image (see :ref:doc:`Tiling and parallelism </tiling>`)
by adding a ``height`` and ``width`` parameter to the function call (N.B. those parameters do not need to be equal).
::

  auto output = mln::morpho::parallel::dilation(input, mln::se::disc(5), 128, 128);

