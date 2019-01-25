#include <mln/core/image/image2d.hpp>
#include <mln/core/image/private/image_operators.hpp>
#include <mln/core/algorithm/for_each.hpp>

#include <utility>

#include <gtest/gtest.h>

TEST(Core, Algorithm_For_Each)
{
  using namespace mln::experimental::ops;

  mln::image2d<uint8_t> ima =
    { {1, 2, 3},
      {4, 5, 6} };

  mln::image2d<uint8_t> ref =
    { {2, 3, 4},
      {5, 6, 7} };

  mln::experimental::for_each(ima, [](uint8_t& x) { x += 1; });
  ASSERT_TRUE(mln::experimental::all(ima == ref));
}


TEST(Core, Algorithm_For_Each_Side_Effects)
{
  using namespace mln::experimental::ops;

  mln::image2d<uint8_t> ima =
    { {1, 2, 3},
      {4, 5, 6} };


  int sum = 0;
  auto g = [&sum](uint8_t v) { sum += v; };
  mln::experimental::for_each(ima, g);
  ASSERT_EQ(6 * 7 / 2, sum);
}
