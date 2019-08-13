#include <mln/core/image/view/maths.hpp>

#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/image/view/transform.hpp>

#include <gtest/gtest.h>


TEST(View, maths_abs)
{
  using namespace mln::view::ops;
  using namespace mln::view::maths;

  mln::experimental::image2d<int> ima = {{0, -1, 2, -3, 4},  //
                                         {-5, 6, -4, 8, -9}, //
                                         {10, -11, 12, -13, 14}};

  mln::experimental::image2d<int> ref = {{0, 1, 2, 3, 4}, //
                                         {5, 6, 4, 8, 9}, //
                                         {10, 11, 12, 13, 14}};

  ASSERT_TRUE(mln::all_of(ref == abs(ima)));


  mln::experimental::image2d<mln::rgb8> ima2 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  mln::experimental::image2d<mln::rgb8> ref2 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  ASSERT_TRUE(mln::all_of(ref2 == abs(ima2)));
}

TEST(View, maths_sqr)
{
  using namespace mln::view::ops;
  using namespace mln::view::maths;

  mln::experimental::image2d<int> ima = {{0, 1, 2, 3, 4}, //
                                         {5, 6, 4, 8, 9}, //
                                         {10, 11, 12, 13, 14}};

  mln::experimental::image2d<int> ref = {{0, 1, 4, 9, 16},     //
                                         {25, 36, 16, 64, 81}, //
                                         {100, 121, 144, 169, 196}};

#ifndef PYLENE_GCC8_WORKAROUND
  ASSERT_TRUE(mln::all_of(ref == sqr(ima)));
#endif

  mln::experimental::image2d<mln::rgb8> ima2 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  mln::experimental::image2d<mln::rgb8> ref2 = {{{1, 4, 9}, {16, 25, 36}, {49, 64, 81}}, //
                                                {{1, 4, 9}, {16, 25, 36}, {49, 64, 81}}, //
                                                {{1, 4, 9}, {16, 25, 36}, {49, 64, 81}}};

  ASSERT_TRUE(mln::all_of(ref2 == sqr(ima2)));
}

TEST(View, maths_pow)
{
  using namespace mln::view::ops;
  using namespace mln::view::maths;

  mln::experimental::image2d<int> ref = {{0, 1, 2, 3, 4}, //
                                         {5, 6, 4, 8, 9}, //
                                         {10, 11, 12, 13, 14}};

  auto ima = mln::view::transform(ref, [](auto x) { return x * x * x; });

#ifndef PYLENE_GCC8_WORKAROUND
  ASSERT_TRUE(mln::all_of(pow(ref, 3) == ima));
#endif


  mln::experimental::image2d<mln::rgb8> ima2 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  mln::experimental::image2d<mln::rgb16> ref2 = {{{1, 8, 27}, {64, 125, 216}, {343, 512, 729}}, //
                                                 {{1, 8, 27}, {64, 125, 216}, {343, 512, 729}}, //
                                                 {{1, 8, 27}, {64, 125, 216}, {343, 512, 729}}};

#ifndef PYLENE_GCC8_WORKAROUND
  ASSERT_TRUE(mln::all_of(ref2 == pow(ima2, 3)));
#endif
}

TEST(View, maths_sqrt)
{
  using namespace mln::view::ops;
  using namespace mln::view::maths;

  mln::experimental::image2d<int> ima = {{0, 1, 4, 9, 16},     //
                                         {25, 36, 16, 64, 81}, //
                                         {100, 121, 144, 169, 196}};

  mln::experimental::image2d<int> ref = {{0, 1, 2, 3, 4}, //
                                         {5, 6, 4, 8, 9}, //
                                         {10, 11, 12, 13, 14}};

  ASSERT_TRUE(mln::all_of(ref == sqrt(ima)));


  mln::experimental::image2d<mln::rgb8> ima2 = {{{1, 4, 9}, {16, 25, 36}, {49, 64, 81}}, //
                                                {{1, 4, 9}, {16, 25, 36}, {49, 64, 81}}, //
                                                {{1, 4, 9}, {16, 25, 36}, {49, 64, 81}}};

  mln::experimental::image2d<mln::rgb8> ref2 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  ASSERT_TRUE(mln::all_of(ref2 == sqrt(ima2)));
}

TEST(View, maths_cbrt)
{
  using namespace mln::view::ops;
  using namespace mln::view::maths;

  mln::experimental::image2d<double> ref = {{0, 1, 2, 3, 4}, //
                                            {5, 6, 4, 8, 9}, //
                                            {10, 11, 12, 13, 14}};

  auto ima = pow(ref, 3);

#ifndef PYLENE_GCC8_WORKAROUND
  ASSERT_TRUE(mln::all_of(equalFP(ref, cbrt(ima), 10e-6)));
#endif

  mln::experimental::image2d<mln::rgb<double>> ref2 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                       {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                       {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  auto ima2 = pow(ref2, 3);

  ASSERT_TRUE(mln::all_of(equalFP(ref2, cbrt(ima2), 10e-6)));
}
