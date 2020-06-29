#include <mln/morpho/rank_filter.hpp>


#include <mln/core/image/ndimage.hpp>
#include <mln/core/se/periodic_line2d.hpp>
#include <mln/core/algorithm/iota.hpp>


#include <fixtures/ImageCompare/image_compare.hpp>

#include <gtest/gtest.h>
#include <ratio>

TEST(Morpho, rank_filter_fill_back)
{
  mln::image2d<uint8_t> ima(10,5);
  mln::iota(ima, 0);

  mln::image2d<uint8_t> ref = {
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},           //
      {0, 1, 2, 3, 4, 5, 6, 7, 8, 9},           //
      {10, 11, 12, 13, 14, 15, 16, 17, 18, 19}, //
      {10, 11, 12, 13, 14, 15, 16, 17, 18, 19}, //
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},           //
  };

  using R = std::ratio<1,5>;

  auto vline = mln::se::periodic_line2d({0, 1}, 2);
  auto out =
    mln::morpho::rank_filter<R>(ima, vline, mln::extension::bm::fill(uint8_t(0)));

  ASSERT_IMAGES_EQ_EXP(out, ref);
}


TEST(Morpho, rank_filter_fill_mirror)
{
  mln::image2d<uint8_t> ima(10,5);
  mln::iota(ima, 0);

  mln::image2d<uint8_t> ref = {
      {10, 11, 12, 13, 14, 15, 16, 17, 18, 19}, //
      {10, 11, 12, 13, 14, 15, 16, 17, 18, 19}, //
      {20, 21, 22, 23, 24, 25, 26, 27, 28, 29}, //
      {30, 31, 32, 33, 34, 35, 36, 37, 38, 39}, //
      {30, 31, 32, 33, 34, 35, 36, 37, 38, 39}, //
  };

  using R = std::ratio<2,5>;

  auto vline = mln::se::periodic_line2d({0, 1}, 2);
  auto out   = mln::morpho::rank_filter<R>(ima, vline, mln::extension::bm::mirror{});

  ASSERT_IMAGES_EQ_EXP(out, ref);
}

