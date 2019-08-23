#include <mln/core/algorithm/paste.hpp>

#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/image/experimental/ndimage.hpp>

#include <vector>

#include <gtest/gtest.h>
#include <fixtures/ImageCompare/image_compare.hpp>

TEST(Core, Experimental_Algorithm_Paste_With_Copy)
{
  mln::experimental::box2d            b({1, 1}, {3, 3});
  mln::experimental::image2d<uint8_t> ima(b);
  mln::experimental::image2d<uint8_t> out(5, 5);
  mln::iota(ima, 101);
  mln::iota(out, 1);
  mln::paste(ima, out);

  mln::experimental::image2d<uint8_t> ref = {{1, 2, 3, 4, 5},        //
                                             {6, 101, 102, 9, 10},   //
                                             {11, 103, 104, 14, 15}, //
                                             {16, 17, 18, 19, 20},   //
                                             {21, 22, 23, 24, 25}};

  ASSERT_IMAGES_EQ_EXP(out, ref);
}


TEST(Core, Experimental_Algorithm_Paste_ROI_copy)
{
  mln::experimental::box2d roi({1, 1}, {3, 3});

  mln::experimental::image2d<uint8_t> ima(5, 5);
  mln::experimental::image2d<uint8_t> out(5, 5);

  mln::iota(ima, 101);
  mln::iota(out, 1);
  mln::paste(ima, roi, out);

  mln::experimental::image2d<uint8_t> ref = {{1, 2, 3, 4, 5},        //
                                             {6, 107, 108, 9, 10},   //
                                             {11, 112, 113, 14, 15}, //
                                             {16, 17, 18, 19, 20},   //
                                             {21, 22, 23, 24, 25}};

  ASSERT_IMAGES_EQ_EXP(out, ref);
}


TEST(Core, Experimental_Algorithm_Paste_ROI_no_copy)
{
  std::vector<mln::experimental::point2d> roi = {{1, 1}, {2, 2}, {3, 3}};
  mln::experimental::image2d<uint8_t> ima(5, 5);
  mln::experimental::image2d<uint8_t> out(5, 5);

  mln::iota(ima, 101);
  mln::iota(out, 1);
  mln::paste(ima, roi, out);

  mln::experimental::image2d<uint8_t> ref = {{1, 2, 3, 4, 5},       //
                                             {6, 107, 8, 9, 10},    //
                                             {11, 12, 113, 14, 15}, //
                                             {16, 17, 18, 119, 20}, //
                                             {21, 22, 23, 24, 25}};

  ASSERT_IMAGES_EQ_EXP(out, ref);
}

