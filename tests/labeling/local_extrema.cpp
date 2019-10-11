#include <mln/labeling/experimental/local_extrema.hpp>

#include <mln/core/algorithm/transform.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/image/view/filter.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>

#include <gtest/gtest.h>



TEST(Morpho, local_minima)
{
  const mln::experimental::image2d<uint8_t> input = {{2, 2, 2, 2, 2},      //
                                                        {40, 30, 30, 30, 40}, //
                                                        {40, 20, 20, 20, 40}, //
                                                        {40, 40, 20, 40, 40}, //
                                                        {1, 5, 20, 5, 1}};

  const mln::experimental::image2d<int8_t> ref = {{1, 1, 1, 1, 1}, //
                                                  {0, 0, 0, 0, 0}, //
                                                  {0, 0, 0, 0, 0}, //
                                                  {0, 0, 0, 0, 0}, //
                                                  {2, 0, 0, 0, 3}};

  int  nlabel;
  auto res = mln::labeling::experimental::local_minima<int8_t>(input, mln::experimental::c4, nlabel);
  ASSERT_EQ(3, nlabel);
  ASSERT_IMAGES_EQ_EXP(ref, res);
}

TEST(Morpho, DISABLED_local_minima_on_view)
{
  const mln::experimental::image2d<uint8_t> input = {{+2, +2, +2, +2, +2}, //
                                                     {40, 30, +2, 30, 40}, //
                                                     {40, 20, +2, 20, 40}, //
                                                     {40, 40, +2, 40, 40}, //
                                                     {+1, +1, +1, +1, +1}};

  const mln::experimental::image2d<int8_t> ref = {{0, 0, 0, 0, 0}, //
                                                  {0, 0, 0, 0, 0}, //
                                                  {0, 1, 0, 2, 0}, //
                                                  {0, 0, 0, 0, 0}, //
                                                  {0, 0, 0, 0, 0}};
  // FIXME: FAILS because of the lifetime of input
  int  nlabel;
  auto input2 = mln::view::filter(input, [](uint8_t x) { return x > 2; });
  auto res   = mln::labeling::experimental::local_minima<int8_t>(input2, mln::experimental::c4, nlabel);

  EXPECT_EQ(2, nlabel);
  EXPECT_EQ(res({1,2}), 1);
  EXPECT_EQ(res({3,2}), 2);

  // FIXME: cross domain comparison is not yet supported
  // ASSERT_IMAGES_EQ_EXP(ref, res);
}




TEST(Morpho, local_maxima)
{
  const mln::experimental::image2d<uint8_t> input = {{2, 2, 2, 2, 2},      //
                                                     {40, 30, 30, 30, 40}, //
                                                     {40, 20, 20, 20, 40}, //
                                                     {40, 40, 20, 40, 40}, //
                                                     {1, 5, 20, 5, 1}};

  const mln::experimental::image2d<int8_t> ref = {{0, 0, 0, 0, 0}, //
                                                  {1, 0, 0, 0, 2}, //
                                                  {1, 0, 0, 0, 2}, //
                                                  {1, 1, 0, 2, 2}, //
                                                  {0, 0, 0, 0, 0}};

  int  nlabel;
  auto res = mln::labeling::experimental::local_maxima<int8_t>(input, mln::experimental::c4, nlabel);
  ASSERT_EQ(2, nlabel);
  ASSERT_IMAGES_EQ_EXP(ref, res);
}
