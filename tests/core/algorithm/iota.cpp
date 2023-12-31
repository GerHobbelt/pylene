#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/operators.hpp>


#include <gtest/gtest.h>

TEST(Core, Algorithm_Iota)
{
  using namespace mln::view::ops;

  mln::image2d<uint8_t> out(4, 3);
  mln::iota(out, 1);

  mln::image2d<uint8_t> ref = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}};

  ASSERT_TRUE(mln::all_of(ref == out));
}
