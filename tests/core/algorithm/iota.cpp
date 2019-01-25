#include <mln/core/image/image2d.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/image/private/image_operators.hpp>


#include <gtest/gtest.h>

TEST(Core, Algorithm_Iota)
{
  using namespace mln::experimental::ops;

  mln::image2d<uint8_t> out(3, 4);
  mln::experimental::iota(out, 1);

  mln::image2d<uint8_t> ref = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}};

  ASSERT_TRUE(mln::experimental::all(ref == out));
}
