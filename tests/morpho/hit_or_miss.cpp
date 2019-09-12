#include <mln/morpho/experimental/hit_or_miss.hpp>

#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/image/view/cast.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/se/mask2d.hpp>

#include <gtest/gtest.h>

#include <fixtures/ImageCompare/image_compare.hpp>


TEST(Morpho, hit_or_miss)
{
  mln::experimental::image2d<bool> ima = {
      {0, 0, 0, 1, 1, 1}, //
      {0, 0, 0, 1, 1, 1}, //
      {0, 0, 0, 1, 1, 1}, //
      {0, 0, 1, 1, 1, 1}, //
      {0, 0, 0, 1, 0, 1}  //
  };

  mln::se::experimental::mask2d win1 = { {0, 1, 1} };
  mln::se::experimental::mask2d win2 = { {1, 0, 0} };

  using namespace mln::view::ops;
  auto out  = mln::morpho::experimental::hit_or_miss(ima, win1, win2);
  auto out2 = mln::morpho::experimental::hit_or_miss((not ima), win2, win1);

  ASSERT_IMAGES_EQ_EXP(out, out2);

  auto out3 = mln::morpho::experimental::hit_or_miss(mln::view::cast<uint8_t>(ima), win1, win2);
  ASSERT_IMAGES_EQ_EXP(mln::view::cast<uint8_t>(out), out3);
}
