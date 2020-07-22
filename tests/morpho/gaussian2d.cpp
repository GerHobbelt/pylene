#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/io/imread.hpp>
#include <mln/morpho/gaussian2d.hpp>
#include <fixtures/ImageCompare/image_compare.hpp>

#include <gtest/gtest.h>


TEST(Morpho, gaussian2d)
{
  mln::image_build_params params;
  params.init_value = uint8_t(0);


  mln::image2d<uint8_t> input(41, 41, params);
  input({10,10}) = 255;


  constexpr float kLineVerticalSigma   = 2.f;
  constexpr float kLineHorizontalSigma = 0.f;

  auto          ret = mln::morpho::gaussian2d(input, kLineHorizontalSigma, kLineVerticalSigma);

  mln::io::imprint(ret);
}
