#include <mln/morpho/watershed.hpp>

#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c4.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>

#include <gtest/gtest.h>


using namespace mln;

TEST(Morpho, watershed_gray)
{
  const mln::image2d<uint8_t> input = {{2, 2, 2, 2, 2},      //
                                                     {40, 30, 30, 30, 40}, //
                                                     {40, 20, 20, 20, 40}, //
                                                     {40, 40, 20, 40, 40}, //
                                                     {1, 5, 20, 5, 1}};

  const mln::image2d<int16_t> ref = {{1, 1, 1, 1, 1}, //
                                                   {1, 1, 1, 1, 1}, //
                                                   {0, 1, 1, 1, 0}, //
                                                   {2, 0, 1, 0, 3}, //
                                                   {2, 2, 0, 3, 3}};

  int  nlabel;
  auto res = mln::morpho::watershed<int16_t>(input, mln::c4, nlabel);
  ASSERT_IMAGES_EQ_EXP(ref, res);
}

TEST(Morpho, watershed_thick)
{
  const mln::image2d<uint8_t> input = {{0, 10, 0, 10, 0},    //
                                                     {0, 10, 10, 10, 10},  //
                                                     {10, 10, 10, 10, 10}, //
                                                     {0, 10, 10, 10, 10},  //
                                                     {0, 10, 0, 10, 0}};

  const mln::image2d<int16_t> ref = {{1, 0, 2, 0, 3}, //
                                                   {1, 1, 0, 3, 3}, //
                                                   {0, 0, 0, 0, 0}, //
                                                   {4, 4, 0, 6, 6}, //
                                                   {4, 0, 5, 0, 6}};

  int  nlabel;
  auto res = mln::morpho::watershed<int16_t>(input, mln::c4, nlabel);
  ASSERT_IMAGES_EQ_EXP(ref, res);
}
