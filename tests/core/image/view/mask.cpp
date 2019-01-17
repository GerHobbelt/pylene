#include <mln/core/image/image2d.hpp>
#include <mln/core/image/private/image_operators.hpp>
#include <mln/core/image/view/mask.hpp>

#include <mln/core/algorithm/fill.hpp>

#include <mln/core/rangev3/foreach.hpp>

#include <helpers.hpp>

#include <gtest/gtest.h>


TEST(View, mask)
{
  using namespace mln::experimental::ops;

  mln::image2d<int> ima = {{0, 1, 2, 3, 4}, //
                           {5, 6, 7, 8, 9}, //
                           {10, 11, 12, 13, 14}};

  mln::image2d<int> ref = {{0, 42, 2, 42, 4},  //
                           {42, 6, 42, 8, 42}, //
                           {10, 42, 12, 42, 14}};


  auto mask = (ima % 2) == 1;


  auto z = mln::view::mask(ima, mask);
  fill(z, 42);

  for (auto p : z.domain())
  {
    ASSERT_EQ(42, ima(p));
    ASSERT_EQ(42, z(p));
  }
  ASSERT_TRUE(mln::experimental::all(ima == ref));
}


TEST(View, mask_twice)
{
  using namespace mln::experimental::ops;

  mln::image2d<int> ima = {{0, 1, 2, 3, 4}, //
                           {5, 6, 7, 8, 9}, //
                           {10, 11, 12, 13, 14}};

  mln::image2d<int> ref = {{0, 42, 2, 3, 4},  //
                           {42, 6, 7, 8, 42}, //
                           {10, 11, 12, 42, 14}};


  auto mask_A = (ima % 2) == 1;
  auto A      = mln::view::mask(ima, mask_A);

  auto mask_B = (A % 4) == 1;
  auto B      = mln::view::mask(A, mask_B);
  fill(B, 42);

  for (auto p : B.domain())
  {
    ASSERT_EQ(42, ima(p));
    ASSERT_EQ(42, B(p));
  }

  ASSERT_TRUE(mln::experimental::all(ima == ref));
}
