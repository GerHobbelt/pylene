#include <mln/core/colors.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/cast.hpp>
#include <mln/core/image/view/rgb.hpp>
#include <mln/core/image/view/transform.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <mln/morpho/gaussian2d.hpp>



template <class I>
auto to_gray(I&& input)
{
  auto grayscale = [](auto v) -> uint8_t { return 0.2126 * v[0] + 0.7152 * v[1] + 0.0722 * v[2]; };
  return mln::view::transform(input, grayscale);
}

int main(int, char** argv)
{
  auto  input = mln::io::imread(argv[1]);
  float sigma = std::atof(argv[2]);

  mln::image2d<float> out;
  if (auto* in = input.cast_to<mln::rgb8, 2>())
    out = mln::morpho::gaussian2d(to_gray(*in), sigma, sigma);
  else if (auto* in = input.cast_to<uint8_t, 2>())
    out = mln::morpho::gaussian2d(*in, sigma, sigma);


  auto o = mln::view::cast<uint8_t>(out);
  mln::io::imsave(o, argv[3]);
}
