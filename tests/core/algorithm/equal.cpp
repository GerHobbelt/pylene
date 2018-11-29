#include <mln/core/grays.hpp>
#include <mln/core/image/image2d.hpp>

#include <mln/core/algorithm/equal.hpp>
#include <mln/core/algorithm/iota.hpp>

#include <gtest/gtest.h>

TEST(Core, Algorithm_Equal)
{
  using namespace mln;

  image2d<uint8> ima(10, 10);
  image2d<uint8> out(10, 10);

  iota(ima, 0);
  iota(out, 0);

  ASSERT_TRUE(equal(ima, out));

  point2d p = {2, 3};
  out(p)    = 12;
  ASSERT_TRUE(not equal(ima, out));
}
