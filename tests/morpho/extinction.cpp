#include <mln/morpho/extinction_transform.hpp>

#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c4.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>
#include <gtest/gtest.h>

using namespace mln;

TEST(Morpho, extinction_extinction_0)
{
  mln::image2d<uint8_t> ima = {
      {0, 3, 2}, //
      {2, 2, 3}, //
      {2, 3, 1}  //
  };

  mln::image2d<uint8_t> ref = {
      {3, 0, 1}, //
      {0, 0, 0}, //
      {0, 0, 2}  //
  };

  auto res = mln::morpho::minima_extinction_transform(ima, mln::c4);
  ASSERT_IMAGES_EQ_EXP(res, ref);
}

TEST(Morpho, extinction_extinction_flatzone)
{
  mln::image2d<uint8_t> ima = {
      {0, 3, 2}, //
      {2, 2, 3}, //
      {2, 1, 1}  //
  };

  // The algorithm does not propagate value in
  // the flat-zone minima
  mln::image2d<uint8_t> ref = {
      {3, 0, 1}, //
      {0, 0, 0}, //
      {0, 1, 0}  //
  };

  auto res = mln::morpho::minima_extinction_transform(ima, mln::c4);
  ASSERT_IMAGES_EQ_EXP(res, ref);
}

/*
TEST(Morpho, extinction_extinction_FlatZone)
{
  image2d<uint8_t> ima = { {0,3,2},
                         {2,2,3},
                         {2,1,1} };

  image2d<uint8_t> ref = { {3,0,1},
                         {0,0,0},
                         {0,1,1} };

  auto E = morpho::extinction(ima, c4);
  ASSERT_TRUE(equal(E, ref));
}
*/
