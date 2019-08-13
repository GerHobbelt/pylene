#include <mln/core/algorithm/copy.hpp>

#include <mln/core/algorithm/equal.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/image/experimental/ndimage.hpp>

#include <gtest/gtest.h>


TEST(Core, Algorithm_Copy)
{
  mln::experimental::image2d<std::uint8_t> ima(10, 10);
  mln::experimental::image2d<std::uint8_t> out(10, 10);
  mln::iota(ima, 0);
  mln::experimental::copy(ima, out);

  ASSERT_TRUE(mln::equal(ima, out));
}
