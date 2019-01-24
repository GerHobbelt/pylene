#include <mln/core/image/image2d.hpp>

#include <mln/core/algorithm/copy.hpp>
#include <mln/core/algorithm/equal.hpp>
#include <mln/core/algorithm/iota.hpp>

#include <gtest/gtest.h>

TEST(Core, Algorithm_Copy)
{
  using namespace mln;

  image2d<uint8> ima(10, 10);
  image2d<uint8> out(10, 10);
  iota(ima, 0);
  copy(ima, out);

  ASSERT_TRUE(equal(ima, out));
}


TEST(Core, Algorithm_Exp_Copy)
{
  mln::image2d<std::uint8_t> ima(10, 10);
  mln::image2d<std::uint8_t> out(10, 10);
  mln::iota(ima, 0);
  mln::experimental::copy(ima, out);

  ASSERT_TRUE(mln::equal(ima, out));
}

