#include <mln/morpho/experimental/area_filter.hpp>

#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighborhood/c8.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>

#include <mln/io/experimental/imprint.hpp>
#include <gtest/gtest.h>


using namespace mln;

TEST(Morpho, area_opening_grayscale)
{
  const mln::experimental::image2d<uint8_t> input = {{+2, +2, +2, +2, +2},      //
                                                     {40, 30, 30, 30, 40}, //
                                                     {40, 20, 20, 20, 40}, //
                                                     {40, 40, 20, 40, 40}, //
                                                     {+1, +5, 20, +5, +1}};
  {
    const mln::experimental::image2d<uint8_t> ref = {{+2, +2, +2, +2, +2}, //
                                                     {30, 30, 30, 30, 30}, //
                                                     {30, 20, 20, 20, 30}, //
                                                     {30, 30, 20, 30, 30}, //
                                                     {+1, +5, 20, +5, +1}};

    auto res = mln::morpho::experimental::area_opening(input, mln::experimental::c4, 5);
    ASSERT_IMAGES_EQ_EXP(ref, res);
  }

  {
    const mln::experimental::image2d<uint8_t> ref = {{+2, +2, +2, +2, +2}, //
                                                     {20, 20, 20, 20, 20}, //
                                                     {20, 20, 20, 20, 20}, //
                                                     {20, 20, 20, 20, 20}, //
                                                     {+1, +5, 20, +5, +1}};

    auto res = mln::morpho::experimental::area_opening(input, mln::experimental::c4, 12);
    ASSERT_IMAGES_EQ_EXP(ref, res);
  }
}

TEST(Morpho, area_opening_binary)
{
  const mln::experimental::image2d<bool> input = {{0, 1, 1, 0, 0, 0, 1}, //
                                                  {1, 0, 1, 0, 0, 1, 1},
                                                  {0, 1, 0, 0, 1, 1, 1}};


  {
    const mln::experimental::image2d<bool> ref = {{0, 1, 1, 0, 0, 0, 1}, //
                                                  {0, 0, 1, 0, 0, 1, 1},
                                                  {0, 0, 0, 0, 1, 1, 1}};

    auto res = mln::morpho::experimental::area_opening(input, mln::experimental::c4, 3);
    ASSERT_IMAGES_EQ_EXP(ref, res);
  }

  {
    const mln::experimental::image2d<bool> ref = {{0, 0, 0, 0, 0, 0, 1}, //
                                                  {0, 0, 0, 0, 0, 1, 1},
                                                  {0, 0, 0, 0, 1, 1, 1}};


    auto res = mln::morpho::experimental::area_opening(input, mln::experimental::c4, 5);
    ASSERT_IMAGES_EQ_EXP(ref, res);
  }


  {
    const mln::experimental::image2d<bool> ref = {{0, 1, 1, 0, 0, 0, 1}, //
                                                  {1, 0, 1, 0, 0, 1, 1},
                                                  {0, 1, 0, 0, 1, 1, 1}};

    auto res = mln::morpho::experimental::area_opening(input, mln::experimental::c8, 5);
    ASSERT_IMAGES_EQ_EXP(ref, res);
  }


  {
    const mln::experimental::image2d<bool> ref = {{0, 0, 0, 0, 0, 0, 1}, //
                                                  {0, 0, 0, 0, 0, 1, 1},
                                                  {0, 0, 0, 0, 1, 1, 1}};

    auto res = mln::morpho::experimental::area_opening(input, mln::experimental::c8, 6);
    ASSERT_IMAGES_EQ_EXP(ref, res);
  }
}
