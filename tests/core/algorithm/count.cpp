#include <mln/core/image/experimental/ndimage.hpp>

#include <mln/core/algorithm/count.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>

#include <gtest/gtest.h>

TEST(Core, Algorithm_Count)
{
  mln::experimental::image2d<int> ima(10, 10);

  mln::iota(ima, 0);

  ASSERT_TRUE(mln::count(ima, 1) == 1);
}

TEST(Core, Algorithm_Count42)
{
  mln::experimental::image2d<int> ima(3, 3);

  mln::fill(ima, 42);

  ASSERT_TRUE(mln::count(ima, 42) == 9);
}
