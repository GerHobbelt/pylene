#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/image/view/maths.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/image/view/transform.hpp>

#include <gtest/gtest.h>


TEST(View, maths_sum)
{
  using namespace mln::view::ops;
  using namespace mln::view::maths;

  mln::experimental::image2d<int> ima = {{0, -1, 2, -3, 4},  //
                                         {-5, 6, -4, 8, -9}, //
                                         {10, -11, 12, -13, 14}};

  mln::experimental::image2d<int> ref = {{0, -1, 2, -3, 4},  //
                                         {-5, 6, -4, 8, -9}, //
                                         {10, -11, 12, -13, 14}};

  ASSERT_TRUE(mln::all_of(ref == sum(ima)));


  mln::experimental::image2d<mln::rgb8> ima2 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  mln::experimental::image2d<int> ref2 = {{6, 15, 24}, //
                                          {6, 15, 24}, //
                                          {6, 15, 24}};

  ASSERT_TRUE(mln::all_of(ref2 == sum(ima2)));
}

TEST(View, maths_prod)
{
  using namespace mln::view::ops;
  using namespace mln::view::maths;

  mln::experimental::image2d<int> ima = {{0, -1, 2, -3, 4},  //
                                         {-5, 6, -4, 8, -9}, //
                                         {10, -11, 12, -13, 14}};

  mln::experimental::image2d<int> ref = {{0, -1, 2, -3, 4},  //
                                         {-5, 6, -4, 8, -9}, //
                                         {10, -11, 12, -13, 14}};

  ASSERT_TRUE(mln::all_of(ref == prod(ima)));


  mln::experimental::image2d<mln::rgb8> ima2 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  mln::experimental::image2d<int> ref2 = {{6, 120, 504}, //
                                          {6, 120, 504}, //
                                          {6, 120, 504}};

  ASSERT_TRUE(mln::all_of(ref2 == prod(ima2)));
}

TEST(View, maths_cross)
{
  using namespace mln::view::ops;
  using namespace mln::view::maths;

  mln::experimental::image2d<mln::rgb8> ima1 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  mln::experimental::image2d<mln::rgb8> ima2 = {{{7, 8, 9}, {4, 5, 6}, {1, 2, 3}}, //
                                                {{7, 8, 9}, {4, 5, 6}, {1, 2, 3}}, //
                                                {{7, 8, 9}, {4, 5, 6}, {1, 2, 3}}};

  mln::experimental::image2d<mln::signed_rgb8> ref = {{{-6, 12, -6}, {0, 0, 0}, {6, -12, 6}}, //
                                                      {{-6, 12, -6}, {0, 0, 0}, {6, -12, 6}}, //
                                                      {{-6, 12, -6}, {0, 0, 0}, {6, -12, 6}}};

  ASSERT_TRUE(mln::all_of(ref == cross(ima1, ima2)));
}

TEST(View, maths_dot)
{
  using namespace mln::view::ops;
  using namespace mln::view::maths;

  mln::experimental::image2d<mln::rgb8> ima1 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  mln::experimental::image2d<mln::rgb8> ima2 = {{{7, 8, 9}, {4, 5, 6}, {1, 2, 3}}, //
                                                {{7, 8, 9}, {4, 5, 6}, {1, 2, 3}}, //
                                                {{7, 8, 9}, {4, 5, 6}, {1, 2, 3}}};

  mln::experimental::image2d<int> ref = {{50, 77, 50}, //
                                         {50, 77, 50}, //
                                         {50, 77, 50}};

  ASSERT_TRUE(mln::all_of(ref == dot(ima1, ima2)));
}

TEST(View, maths_min)
{
  using namespace mln::view::ops;
  using namespace mln::view::maths;

  mln::experimental::image2d<int> ima = {{0, -1, 2, -3, 4},  //
                                         {-5, 6, -4, 8, -9}, //
                                         {10, -11, 12, -13, 14}};

  mln::experimental::image2d<int> ref = {{0, -1, 2, -3, 4},  //
                                         {-5, 6, -4, 8, -9}, //
                                         {10, -11, 12, -13, 14}};

  ASSERT_TRUE(mln::all_of(ref == min(ima)));


  mln::experimental::image2d<mln::rgb8> ima2 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  mln::experimental::image2d<int> ref2 = {{1, 4, 7}, //
                                          {1, 4, 7}, //
                                          {1, 4, 7}};

  ASSERT_TRUE(mln::all_of(ref2 == min(ima2)));
}

TEST(View, maths_max)
{
  using namespace mln::view::ops;
  using namespace mln::view::maths;

  mln::experimental::image2d<int> ima = {{0, -1, 2, -3, 4},  //
                                         {-5, 6, -4, 8, -9}, //
                                         {10, -11, 12, -13, 14}};

  mln::experimental::image2d<int> ref = {{0, -1, 2, -3, 4},  //
                                         {-5, 6, -4, 8, -9}, //
                                         {10, -11, 12, -13, 14}};

  ASSERT_TRUE(mln::all_of(ref == max(ima)));


  mln::experimental::image2d<mln::rgb8> ima2 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  mln::experimental::image2d<int> ref2 = {{3, 6, 9}, //
                                          {3, 6, 9}, //
                                          {3, 6, 9}};

  ASSERT_TRUE(mln::all_of(ref2 == max(ima2)));
}
