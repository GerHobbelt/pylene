#include <mln/core/image/image2d.hpp>

#include <mln/core/algorithm/count_if.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>

#include <gtest/gtest.h>

TEST(Core, Algorithm_CountIf)
{
  using namespace mln;
  using namespace mln::experimental;

  image2d<int> ima(10, 10);

  mln::iota(ima, 0);

  ASSERT_TRUE(count_if(ima, [](auto v) { return v == 1; }) == 1);
}

TEST(Core, Algorithm_CountIf42)
{
  using namespace mln;
  using namespace mln::experimental;

  image2d<int> ima(3, 3);

  mln::fill(ima, 42);

  ASSERT_TRUE(count_if(ima, [](auto v) { return v == 42; }) == 9);
}