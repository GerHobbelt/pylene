#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/image/view/rgb.hpp>

#include <gtest/gtest.h>


TEST(View, rgb_red)
{
  using namespace mln::view::ops;

  mln::experimental::image2d<mln::rgb8> ima = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                 {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                 {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  auto ima_red = mln::view::red(ima);

  mln::experimental::image2d<unsigned> ref = {{1, 4, 7}, //
                                {1, 4, 7}, //
                                {1, 4, 7}};

  ASSERT_TRUE(mln::all_of(ref == ima_red));

  mln::fill(ima_red, 42);

  mln::experimental::image2d<mln::rgb8> ref2 = {{{42, 2, 3}, {42, 5, 6}, {42, 8, 9}}, //
                                  {{42, 2, 3}, {42, 5, 6}, {42, 8, 9}}, //
                                  {{42, 2, 3}, {42, 5, 6}, {42, 8, 9}}};

  ASSERT_TRUE(mln::all_of(ref2 == ima));
}

TEST(View, rgb_green)
{
  using namespace mln::view::ops;

  mln::experimental::image2d<mln::rgb8> ima = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                 {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                 {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  auto ima_green = mln::view::green(ima);

  mln::experimental::image2d<unsigned> ref = {{2, 5, 8}, //
                                {2, 5, 8}, //
                                {2, 5, 8}};

  ASSERT_TRUE(mln::all_of(ref == ima_green));

  mln::fill(ima_green, 42);

  mln::experimental::image2d<mln::rgb8> ref2 = {{{1, 42, 3}, {4, 42, 6}, {7, 42, 9}}, //
                                  {{1, 42, 3}, {4, 42, 6}, {7, 42, 9}}, //
                                  {{1, 42, 3}, {4, 42, 6}, {7, 42, 9}}};

  ASSERT_TRUE(mln::all_of(ref2 == ima));
}

TEST(View, rgb_blue)
{
  using namespace mln::view::ops;

  mln::experimental::image2d<mln::rgb8> ima = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                 {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                 {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  auto ima_blue = mln::view::blue(ima);

  mln::experimental::image2d<unsigned> ref = {{3, 6, 9}, //
                                {3, 6, 9}, //
                                {3, 6, 9}};

  ASSERT_TRUE(mln::all_of(ref == ima_blue));

  mln::fill(ima_blue, 42);

  mln::experimental::image2d<mln::rgb8> ref2 = {{{1, 2, 42}, {4, 5, 42}, {7, 8, 42}}, //
                                  {{1, 2, 42}, {4, 5, 42}, {7, 8, 42}}, //
                                  {{1, 2, 42}, {4, 5, 42}, {7, 8, 42}}};

  ASSERT_TRUE(mln::all_of(ref2 == ima));
}
