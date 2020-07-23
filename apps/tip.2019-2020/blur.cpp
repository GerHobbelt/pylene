#include <mln/core/image/ndimage.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <mln/morpho/gaussian2d.hpp>
#include <mln/core/image/view/cast.hpp>
#include <mln/core/image/view/rgb.hpp>
#include <mln/core/colors.hpp>

int main(int, char** argv)
{
  auto input = mln::io::imread(argv[1]);
  float sigma = std::atof(argv[2]);

  mln::image2d<float> out;
  if (auto* in = input.cast_to<mln::rgb8, 2>())
    out = mln::morpho::gaussian2d(mln::view::red(*in), sigma, sigma);
  else if (auto* in = input.cast_to<uint8_t, 2>())
    out = mln::morpho::gaussian2d(*in, sigma, sigma);


  auto o = mln::view::cast<uint8_t>(out);
  mln::io::imsave(o, argv[3]);
}
