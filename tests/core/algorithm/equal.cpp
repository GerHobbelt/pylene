#include <mln/core/algorithm/equal.hpp>

#include <mln/core/algorithm/iota.hpp>
#include <mln/core/image/ndimage.hpp>

#include <gtest/gtest.h>

TEST(Core, Algorithm_Equal)
{
  mln::image2d<uint8_t> ima(10, 10);
  mln::image2d<uint8_t> out(10, 10);

  mln::iota(ima, 0);
  mln::iota(out, 0);

  ASSERT_TRUE(mln::equal(ima, out));

  mln::point2d p = {2, 3};
  out(p)         = 12;
  ASSERT_TRUE(not mln::equal(ima, out));
}

TEST(Core, Algorithm_ExperimentalEqual)
{
  using namespace mln;

  image2d<uint8> ima(10, 10);
  image2d<uint8> out(10, 10);

  iota(ima, 0);
  iota(out, 0);

  ASSERT_TRUE(experimental::equal(ima, out));

  point2d p = {2, 3};
  out(p)    = 12;
  ASSERT_TRUE(not experimental::equal(ima, out));
}
