#include <mln/core/algorithm/equal.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/morpho/extinction.hpp>

#include <gtest/gtest.h>

using namespace mln;

TEST(Morpho, extinction_extinction_0)
{
  image2d<uint8> ima = {
      {0, 3, 2}, //
      {2, 2, 3}, //
      {2, 3, 1}  //
  };

  image2d<uint8> ref = {
      {3, 0, 1}, //
      {0, 0, 0}, //
      {0, 0, 2}  //
  };

  auto E = morpho::extinction(ima, c4);
  ASSERT_TRUE(equal(E, ref));
}

TEST(Morpho, extinction_extinction_flatzone)
{
  image2d<uint8> ima = {
      {0, 3, 2}, //
      {2, 2, 3}, //
      {2, 1, 1}  //
  };

  // The algorithm does not propagate value in
  // the flat-zone minima
  image2d<uint8> ref = {
      {3, 0, 1}, //
      {0, 0, 0}, //
      {0, 0, 1}  //
  };

  auto E = morpho::extinction(ima, c4);
  ASSERT_TRUE(equal(E, ref));
}

/*
TEST(Morpho, extinction_extinction_FlatZone)
{
  image2d<uint8> ima = { {0,3,2},
                         {2,2,3},
                         {2,1,1} };

  image2d<uint8> ref = { {3,0,1},
                         {0,0,0},
                         {0,1,1} };

  auto E = morpho::extinction(ima, c4);
  ASSERT_TRUE(equal(E, ref));
}
*/
