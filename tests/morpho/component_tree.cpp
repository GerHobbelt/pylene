#include <mln/core/image/ndimage.hpp>
#include <mln/morpho/alphatree.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>

#include <gtest/gtest.h>

TEST(Morpho, SaliencyAlphaTree)
{
  mln::image2d<std::uint8_t> input = {
      {1, 1, 3, 1}, //
      {2, 6, 8, 7}, //
      {5, 3, 2, 2}  //
  };

  mln::image2d<double> expected = {
      {0, 0, 0, 2, 0, 2, 0}, //
      {1, 3, 3, 3, 3, 3, 3}, //
      {0, 3, 0, 2, 0, 1, 0}, //
      {3, 3, 3, 3, 3, 3, 3}, //
      {0, 2, 0, 1, 0, 0, 0}  //
  };

  auto [t, nm] =
      mln::morpho::alphatree(input, mln::c4, [](auto a, auto b) -> double { return mln::functional::l2dist(a, b); });
  const auto res = t.saliency(nm, ::ranges::make_span(t.values.data(), t.values.size()));

  ASSERT_IMAGES_EQ_EXP(res, expected);
}