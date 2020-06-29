#include <mln/morpho/dynamic_filter.hpp>


#include <mln/core/image/ndimage.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighborhood/c8.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>

#include <gtest/gtest.h>



TEST(Morpho, dynamic_opening_grayscale)
{
  const mln::image2d<uint8_t> input = {{+2, +2, +2, +2, +2},      //
                                                     {+4, +2, +5, +4, +2}, //
                                                     {+2, +2, +2, +7, +7}, //
                                                     {+5, +4, +6, +5, +2}, //
                                                     {+3, +4, +4, +9, +9}};
  {
    const mln::image2d<uint8_t> ref = {{2, 2, 2, 2, 2}, //
                                                     {2, 2, 4, 4, 2},
                                                     {2, 2, 2, 5, 5},
                                                     {4, 4, 5, 5, 2},
                                                     {3, 4, 4, 5, 5}};

    auto res = mln::morpho::dynamic_opening(input, mln::c4, 4);
    ASSERT_IMAGES_EQ_EXP(ref, res);
  }
}

