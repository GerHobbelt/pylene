#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/transform.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/view/operators.hpp>

#include <utility>

#include <gtest/gtest.h>

TEST(Core, Algorithm_Transform)
{
  using namespace mln::view::ops;

  mln::image2d<uint8_t> ima = {{1, 2, 3}, {4, 5, 6}};

  mln::image2d<uint8_t> ref = {{2, 3, 4}, {5, 6, 7}};

  auto out = mln::transform(ima, [](uint8_t x) -> uint8_t { return x + 1; });
  ASSERT_TRUE(mln::all_of(out == ref));
}


TEST(Core, Algorithm_Transform_LValue)
{
  using namespace mln::view::ops;
  using V = std::pair<uint8_t, uint8_t>;


  mln::image2d<V>       ima = {{{1, 0}, {2, 0}, {3, 0}}, {{4, 0}, {5, 0}, {6, 0}}};
  mln::image2d<uint8_t> ref = {{1, 2, 3}, {4, 5, 6}};


  mln::image2d<uint8_t> out = mln::transform(ima, &V::first);
  ASSERT_TRUE(mln::all_of(out == ref));
}
