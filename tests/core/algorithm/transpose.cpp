#include <mln/core/algorithm/transpose.hpp>
#include <mln/core/image/ndimage.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>

#include <iostream>
#include <utility>

#include <gtest/gtest.h>


TEST(Core, Algorithm_Transpose2d)
{
  mln::image2d<uint8_t> ima = {{1, 2, 3}, //
                               {4, 5, 6}, //
                               {7, 8, 9}};

  mln::image2d<uint8_t> ref = {{1, 4, 7}, //
                               {2, 5, 8}, //
                               {3, 6, 9}};


  mln::image2d<uint8_t> out;
  mln::resize(out, ima);
  mln::transpose(ima, out);

  ASSERT_IMAGES_EQ_EXP(out, ref);
}

TEST(Core, Algorithm_Transpose3d)
{
  mln::image3d<uint8_t> ima = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}},          //
                               {{11, 12, 13}, {14, 15, 16}, {17, 18, 19}}, //
                               {{21, 22, 23}, {24, 25, 26}, {27, 28, 29}}};

  mln::image3d<uint8_t> ref = {{{1, 4, 7}, {11, 14, 17}, {21, 24, 27}}, //
                               {{2, 5, 8}, {12, 15, 18}, {22, 25, 28}}, //
                               {{3, 6, 9}, {13, 16, 19}, {23, 26, 29}}};


  mln::image3d<uint8_t> out;
  mln::resize(out, ima);
  mln::transpose(ima, out);

  ASSERT_IMAGES_EQ_EXP(out, ref);
}