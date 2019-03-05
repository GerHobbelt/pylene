#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/generate.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/private/image_operators.hpp>

#include <gtest/gtest.h>

TEST(Core, Algorithm_Generate)
{
  using namespace mln::experimental::ops;

  // Another impl of mln::iota
  int  i   = 1;
  auto gen = [&i]() { return i++; };

  mln::image2d<uint8_t> out(3, 4);
  mln::experimental::generate(out, gen);

  mln::image2d<uint8_t> ref = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}};

  ASSERT_TRUE(mln::all_of(ref == out));
}
