#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/clone.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/private/image_operators.hpp>

#include <gtest/gtest.h>

TEST(Core, Algorithm_Clone)
{
  using namespace mln::experimental::ops;

  const mln::image2d<uint8_t> ima = {{1, 2, 3}, {4, 5, 6}};
  auto                        out = mln::clone(ima);

  // Writing does not affect ima
  out({0, 0}) = 69;


  const mln::image2d<uint8_t> ref = {{69, 2, 3}, {4, 5, 6}};
  ASSERT_TRUE(mln::all_of(out == ref));
}
