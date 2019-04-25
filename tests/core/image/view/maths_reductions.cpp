#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/view/maths.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/image/view/transform.hpp>

#include <gtest/gtest.h>


TEST(View, maths_sum)
{
  using namespace mln;
  using namespace view::ops;
  using namespace view::maths;

  image2d<int> ima = {{0, -1, 2, -3, 4},  //
                      {-5, 6, -4, 8, -9}, //
                      {10, -11, 12, -13, 14}};

  image2d<int> ref = {{0, -1, 2, -3, 4},  //
                      {-5, 6, -4, 8, -9}, //
                      {10, -11, 12, -13, 14}};

  ASSERT_TRUE(all_of(ref == sum(ima)));


  image2d<mln::rgb8> ima2 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  image2d<int> ref2 = {{6, 15, 24}, //
                       {6, 15, 24}, //
                       {6, 15, 24}};

  ASSERT_TRUE(all_of(ref2 == sum(ima2)));
}

TEST(View, maths_prod)
{
  using namespace mln;
  using namespace view::ops;
  using namespace view::maths;

  image2d<int> ima = {{0, -1, 2, -3, 4},  //
                      {-5, 6, -4, 8, -9}, //
                      {10, -11, 12, -13, 14}};

  image2d<int> ref = {{0, -1, 2, -3, 4},  //
                      {-5, 6, -4, 8, -9}, //
                      {10, -11, 12, -13, 14}};

  ASSERT_TRUE(all_of(ref == prod(ima)));


  image2d<mln::rgb8> ima2 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  image2d<int> ref2 = {{6, 120, 504}, //
                       {6, 120, 504}, //
                       {6, 120, 504}};

  ASSERT_TRUE(all_of(ref2 == prod(ima2)));
}

TEST(View, maths_cross)
{
  using namespace mln;
  using namespace view::ops;
  using namespace view::maths;

  image2d<mln::rgb8> ima1 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  image2d<mln::rgb8> ima2 = {{{7, 8, 9}, {4, 5, 6}, {1, 2, 3}}, //
                             {{7, 8, 9}, {4, 5, 6}, {1, 2, 3}}, //
                             {{7, 8, 9}, {4, 5, 6}, {1, 2, 3}}};

  image2d<mln::signed_rgb8> ref = {{{-6, 12, -6}, {0, 0, 0}, {6, -12, 6}}, //
                                   {{-6, 12, -6}, {0, 0, 0}, {6, -12, 6}}, //
                                   {{-6, 12, -6}, {0, 0, 0}, {6, -12, 6}}};

  ASSERT_TRUE(all_of(ref == cross(ima1, ima2)));
}

TEST(View, maths_dot)
{
  using namespace mln;
  using namespace view::ops;
  using namespace view::maths;

  image2d<mln::rgb8> ima1 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  image2d<mln::rgb8> ima2 = {{{7, 8, 9}, {4, 5, 6}, {1, 2, 3}}, //
                             {{7, 8, 9}, {4, 5, 6}, {1, 2, 3}}, //
                             {{7, 8, 9}, {4, 5, 6}, {1, 2, 3}}};

  image2d<int> ref = {{50, 77, 50}, //
                      {50, 77, 50}, //
                      {50, 77, 50}};

  ASSERT_TRUE(all_of(ref == dot(ima1, ima2)));
}

TEST(View, maths_min)
{
  using namespace mln;
  using namespace view::ops;
  using namespace view::maths;

  image2d<int> ima = {{0, -1, 2, -3, 4},  //
                      {-5, 6, -4, 8, -9}, //
                      {10, -11, 12, -13, 14}};

  image2d<int> ref = {{0, -1, 2, -3, 4},  //
                      {-5, 6, -4, 8, -9}, //
                      {10, -11, 12, -13, 14}};

  ASSERT_TRUE(all_of(ref == min(ima)));


  image2d<mln::rgb8> ima2 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  image2d<int> ref2 = {{1, 4, 7}, //
                       {1, 4, 7}, //
                       {1, 4, 7}};

  ASSERT_TRUE(all_of(ref2 == min(ima2)));
}

TEST(View, maths_max)
{
  using namespace mln;
  using namespace view::ops;
  using namespace view::maths;

  image2d<int> ima = {{0, -1, 2, -3, 4},  //
                      {-5, 6, -4, 8, -9}, //
                      {10, -11, 12, -13, 14}};

  image2d<int> ref = {{0, -1, 2, -3, 4},  //
                      {-5, 6, -4, 8, -9}, //
                      {10, -11, 12, -13, 14}};

  ASSERT_TRUE(all_of(ref == max(ima)));


  image2d<mln::rgb8> ima2 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  image2d<int> ref2 = {{3, 6, 9}, //
                       {3, 6, 9}, //
                       {3, 6, 9}};

  ASSERT_TRUE(all_of(ref2 == max(ima2)));
}
