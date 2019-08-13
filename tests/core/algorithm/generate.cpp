#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/generate.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/image/view/operators.hpp>

#include <gtest/gtest.h>

TEST(Core, Algorithm_Generate)
{
  using namespace mln::view::ops;

  // Another impl of mln::iota
  int  i   = 1;
  auto gen = [&i]() { return i++; };

  mln::experimental::image2d<uint8_t> out(4, 3);
  mln::generate(out, gen);

  mln::experimental::image2d<uint8_t> ref = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}};

  ASSERT_TRUE(mln::all_of(ref == out));
}
