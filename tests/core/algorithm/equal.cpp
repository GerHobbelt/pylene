#include <mln/core/grays.hpp>
#include <mln/core/image/image2d.hpp>

#include <mln/core/algorithm/equal.hpp>
#include <mln/core/algorithm/iota.hpp>

#include <gtest/gtest.h>

TEST(Core, Algorithm_Equal)
{
  mln::image2d<mln::uint8> ima(10, 10);
  mln::image2d<mln::uint8> out(10, 10);

  mln::iota(ima, 0);
  mln::iota(out, 0);

  ASSERT_TRUE(mln::equal(ima, out));

  mln::point2d p = {2, 3};
  out(p)         = 12;
  ASSERT_TRUE(not mln::equal(ima, out));
}

TEST(Core, Algorithm_ExperimentalEqual)
{
  mln::image2d<mln::uint8> ima(10, 10);
  mln::image2d<mln::uint8> out(10, 10);

  mln::iota(ima, 0);
  mln::iota(out, 0);

  ASSERT_TRUE(mln::equal(ima, out));

  mln::point2d p = {2, 3};
  out(p)         = 12;
  ASSERT_TRUE(not mln::equal(ima, out));
}
