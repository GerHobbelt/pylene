#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/view/maths.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/image/view/transform.hpp>

#include <gtest/gtest.h>


TEST(View, maths_abs)
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

  ASSERT_TRUE(all_of(ref == abs(ima)));


  image2d<mln::rgb8> ima2 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  image2d<mln::rgb8> ref2 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  ASSERT_TRUE(all_of(ref2 == abs(ima2)));
}

TEST(View, maths_sqr)
{
  using namespace mln;
  using namespace view::ops;
  using namespace view::maths;

  image2d<int> ima = {{0, 1, 2, 3, 4}, //
                      {5, 6, 4, 8, 9}, //
                      {10, 11, 12, 13, 14}};

  image2d<int> ref = {{0, 1, 4, 9, 16},     //
                      {25, 36, 16, 64, 81}, //
                      {100, 121, 144, 169, 196}};

  ASSERT_TRUE(all_of(ref == sqr(ima)));


  image2d<mln::rgb8> ima2 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  image2d<mln::rgb8> ref2 = {{{1, 4, 9}, {16, 25, 36}, {49, 64, 81}}, //
                             {{1, 4, 9}, {16, 25, 36}, {49, 64, 81}}, //
                             {{1, 4, 9}, {16, 25, 36}, {49, 64, 81}}};

  ASSERT_TRUE(all_of(ref2 == sqr(ima2)));
}

TEST(View, maths_pow)
{
  using namespace mln;
  using namespace view::ops;
  using namespace view::maths;

  image2d<int> ref = {{0, 1, 2, 3, 4}, //
                      {5, 6, 4, 8, 9}, //
                      {10, 11, 12, 13, 14}};

  auto ima = view::transform(ref, [](auto x) { return x * x * x; });

  ASSERT_TRUE(all_of(pow(ref, 3) == ima));


  image2d<mln::rgb8> ima2 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  image2d<mln::rgb16> ref2 = {{{1, 8, 27}, {64, 125, 216}, {343, 512, 729}}, //
                              {{1, 8, 27}, {64, 125, 216}, {343, 512, 729}}, //
                              {{1, 8, 27}, {64, 125, 216}, {343, 512, 729}}};

  ASSERT_TRUE(all_of(ref2 == pow(ima2, 3)));
}

TEST(View, maths_sqrt)
{
  using namespace mln;
  using namespace view::ops;
  using namespace view::maths;

  image2d<int> ima = {{0, 1, 4, 9, 16},     //
                      {25, 36, 16, 64, 81}, //
                      {100, 121, 144, 169, 196}};

  image2d<int> ref = {{0, 1, 2, 3, 4}, //
                      {5, 6, 4, 8, 9}, //
                      {10, 11, 12, 13, 14}};

  ASSERT_TRUE(all_of(ref == sqrt(ima)));


  image2d<mln::rgb8> ima2 = {{{1, 4, 9}, {16, 25, 36}, {49, 64, 81}}, //
                             {{1, 4, 9}, {16, 25, 36}, {49, 64, 81}}, //
                             {{1, 4, 9}, {16, 25, 36}, {49, 64, 81}}};

  image2d<mln::rgb8> ref2 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                             {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  ASSERT_TRUE(all_of(ref2 == sqrt(ima2)));
}

TEST(View, maths_cbrt)
{
  using namespace mln;
  using namespace view::ops;
  using namespace view::maths;

  image2d<double> ref = {{0, 1, 2, 3, 4}, //
                         {5, 6, 4, 8, 9}, //
                         {10, 11, 12, 13, 14}};

  auto ima = pow(ref, 3);

  ASSERT_TRUE(all_of(equalFP(ref, cbrt(ima), 10e-6)));


  image2d<mln::rgb<double>> ref2 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                    {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                    {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  auto ima2 = pow(ref2, 3);

  ASSERT_TRUE(all_of(equalFP(ref2, cbrt(ima2), 10e-6)));
}
