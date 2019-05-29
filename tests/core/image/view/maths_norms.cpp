#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/view/maths.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/image/view/transform.hpp>

#include <cmath>

#include <gtest/gtest.h>


TEST(View, maths_l0norm)
{
  using namespace mln;
  using namespace view::ops;
  using namespace view::maths;

  image2d<int> ima = {{0, -1, 2, -3, 4},  //
                      {-5, 6, -4, 8, -9}, //
                      {10, -11, 12, -13, 14}};

  image2d<int> ref = {{0, 1, 2, 3, 4}, //
                      {5, 6, 4, 8, 9}, //
                      {10, 11, 12, 13, 14}};

  ASSERT_TRUE(all_of(ref == l0norm(ima)));


  image2d<mln::rgb8> ima2 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  image2d<int> ref2 = {{1, 4, 7}, //
                       {1, 4, 7}, //
                       {1, 4, 7}};

  ASSERT_TRUE(all_of(ref2 == l0norm(ima2)));
}

TEST(View, maths_l1norm)
{
  using namespace mln;
  using namespace view::ops;
  using namespace view::maths;

  image2d<int> ima = {{0, -1, 2, -3, 4},  //
                      {-5, 6, -4, 8, -9}, //
                      {10, -11, 12, -13, 14}};

  image2d<int> ref = {{0, 1, 2, 3, 4}, //
                      {5, 6, 4, 8, 9}, //
                      {10, 11, 12, 13, 14}};

  ASSERT_TRUE(all_of(ref == l1norm(ima)));


  image2d<mln::rgb8> ima2 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  image2d<int> ref2 = {{6, 15, 24}, //
                       {6, 15, 24}, //
                       {6, 15, 24}};

  // FIXME: gcc8 random compilation failure in release
  // ASSERT_TRUE(all_of(ref2 == l1norm(ima2)));
}

TEST(View, maths_l2norm)
{
  using namespace mln;
  using namespace view::ops;
  using namespace view::maths;

  image2d<int> ima = {{0, -1, 2, -3, 4},  //
                      {-5, 6, -4, 8, -9}, //
                      {10, -11, 12, -13, 14}};

  image2d<int> ref = {{0, 1, 2, 3, 4}, //
                      {5, 6, 4, 8, 9}, //
                      {10, 11, 12, 13, 14}};

  ASSERT_TRUE(all_of(ref == l2norm(ima)));


  image2d<mln::rgb8> ima2 = {{{1, 2, 2}, {2, 4, 4}, {3, 6, 6}}, //
                             {{1, 2, 2}, {2, 4, 4}, {3, 6, 6}}, //
                             {{1, 2, 2}, {2, 4, 4}, {3, 6, 6}}};

  image2d<int> ref2 = {{3, 6, 9}, //
                       {3, 6, 9}, //
                       {3, 6, 9}};

  ASSERT_TRUE(all_of(ref2 == l2norm(ima2)));
}

TEST(View, maths_lpnorm)
{
  using namespace mln;
  using namespace view::ops;
  using namespace view::maths;

  image2d<int> ima = {{0, -1, 2, -3, 4},  //
                      {-5, 6, -4, 8, -9}, //
                      {10, -11, 12, -13, 14}};

  image2d<int> ref = {{0, 1, 2, 3, 4}, //
                      {5, 6, 4, 8, 9}, //
                      {10, 11, 12, 13, 14}};

  ASSERT_TRUE(all_of(ref == lpnorm<4>(ima)));


  image2d<mln::rgb8> ima2 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  image2d<double> ref2 = {{std::pow(98, 1. / 4.), std::pow(2177, 1. / 4.), std::pow(13058, 1. / 4.)}, //
                          {std::pow(98, 1. / 4.), std::pow(2177, 1. / 4.), std::pow(13058, 1. / 4.)}, //
                          {std::pow(98, 1. / 4.), std::pow(2177, 1. / 4.), std::pow(13058, 1. / 4.)}};

  ASSERT_TRUE(all_of(ref2 == lpnorm<4>(ima2)));
}

TEST(View, maths_l2norm_sqr)
{
  using namespace mln;
  using namespace view::ops;
  using namespace view::maths;

  image2d<int> ima = {{0, -1, 2, -3, 4},  //
                      {-5, 6, -4, 8, -9}, //
                      {10, -11, 12, -13, 14}};

  image2d<int> ref = {{0, 1, 4, 9, 16},     //
                      {25, 36, 16, 64, 81}, //
                      {100, 121, 144, 169, 196}};

  ASSERT_TRUE(all_of(ref == l2norm_sqr(ima)));


  image2d<mln::rgb8> ima2 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  image2d<int> ref2 = {{14, 77, 194}, //
                       {14, 77, 194}, //
                       {14, 77, 194}};

  ASSERT_TRUE(all_of(ref2 == l2norm_sqr(ima2)));
}

TEST(View, maths_linfnorm)
{
  using namespace mln;
  using namespace view::ops;
  using namespace view::maths;

  image2d<int> ima = {{0, -1, 2, -3, 4},  //
                      {-5, 6, -4, 8, -9}, //
                      {10, -11, 12, -13, 14}};

  image2d<int> ref = {{0, 1, 2, 3, 4}, //
                      {5, 6, 4, 8, 9}, //
                      {10, 11, 12, 13, 14}};

  ASSERT_TRUE(all_of(ref == linfnorm(ima)));


  image2d<mln::rgb8> ima2 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  image2d<int> ref2 = {{3, 6, 9}, //
                       {3, 6, 9}, //
                       {3, 6, 9}};

  ASSERT_TRUE(all_of(ref2 == linfnorm(ima2)));
}
