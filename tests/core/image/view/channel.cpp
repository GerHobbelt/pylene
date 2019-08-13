#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/image/view/channel.hpp>
#include <mln/core/image/view/operators.hpp>

#include <gtest/gtest.h>

TEST(View, channel_static)
{
  using namespace mln::view::ops;

  mln::experimental::image2d<mln::rgb8> ima = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                               {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                               {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  auto ima_ch1 = mln::view::channel<1>(ima);

  mln::experimental::image2d<unsigned> ref = {{2, 5, 8}, //
                                              {2, 5, 8}, //
                                              {2, 5, 8}};

  ASSERT_TRUE(mln::all_of(ref == ima_ch1));

  mln::fill(ima_ch1, 42);

  mln::experimental::image2d<mln::rgb8> ref2 = {{{1, 42, 3}, {4, 42, 6}, {7, 42, 9}}, //
                                                {{1, 42, 3}, {4, 42, 6}, {7, 42, 9}}, //
                                                {{1, 42, 3}, {4, 42, 6}, {7, 42, 9}}};

  ASSERT_TRUE(mln::all_of(ref2 == ima));
}

TEST(View, channel_dynamic)
{
  using namespace mln::view::ops;

  mln::experimental::image2d<mln::rgb8> ima = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                               {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                               {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  auto ima_ch2 = mln::view::channel(ima, 2);

  mln::experimental::image2d<unsigned> ref = {{3, 6, 9}, //
                                              {3, 6, 9}, //
                                              {3, 6, 9}};

  ASSERT_TRUE(mln::all_of(ref == ima_ch2));

  mln::fill(ima_ch2, 42);

  mln::experimental::image2d<mln::rgb8> ref2 = {{{1, 2, 42}, {4, 5, 42}, {7, 8, 42}}, //
                                                {{1, 2, 42}, {4, 5, 42}, {7, 8, 42}}, //
                                                {{1, 2, 42}, {4, 5, 42}, {7, 8, 42}}};

  ASSERT_TRUE(mln::all_of(ref2 == ima));
}
