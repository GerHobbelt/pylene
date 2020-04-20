#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/operators.hpp>

#include <gtest/gtest.h>

TEST(Core, Algorithm_Fill)
{
  using namespace mln::view::ops;

  mln::image2d<std::uint8_t> ima(10, 10);
  mln::fill(ima, 69);

  ASSERT_TRUE(mln::all_of(ima == 69));
}
