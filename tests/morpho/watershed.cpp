#include <mln/morpho/watershed.hpp>

#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighborhood/c8.hpp>

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

TEST(Morpho, watershed_from_markers_c4)
{
  const mln::image2d<std::uint8_t> input{
      {255, 255, 255, 255, 255, 255, 255, 255}, //
      {255, 0, 0, 0, 255, 0, 0, 255},           //
      {255, 0, 0, 0, 255, 0, 0, 255},           //
      {255, 0, 0, 0, 255, 255, 255, 255},       //
      {255, 255, 255, 255, 255, 3, 2, 255}      //
  };

  const mln::image2d<std::uint8_t> seeds{
      {0, 0, 0, 0, 0, 0, 0, 0}, //
      {0, 3, 0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 5, 0, 0}, //
      {0, 0, 0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 0, 0, 0}  //
  };

  const mln::image2d<std::int16_t> ref{
      {1, 1, 1, 1, 0, 2, 2, 2}, //
      {1, 1, 1, 1, 0, 2, 2, 2}, //
      {1, 1, 1, 1, 0, 2, 2, 2}, //
      {1, 1, 1, 1, 0, 2, 2, 2}, //
      {1, 1, 1, 1, 0, 2, 2, 2}, //
  };

  int  nlabel;
  auto out = mln::morpho::watershed_from_markers<std::int16_t>(input, mln::c4, seeds, nlabel);
  ASSERT_IMAGES_EQ_EXP(out, ref);
  ASSERT_EQ(nlabel, 2);
}

TEST(Morpho, watershed_from_markers_c8)
{
  const mln::image2d<std::uint8_t> input{
      {255, 255, 255, 255, 255, 255, 255, 255}, //
      {255, 0, 0, 0, 255, 0, 0, 255},           //
      {255, 0, 0, 0, 255, 0, 0, 255},           //
      {255, 0, 0, 0, 255, 255, 255, 255},       //
      {255, 255, 255, 255, 0, 0, 2, 255}        //
  };

  const mln::image2d<std::uint8_t> seeds{
      {0, 0, 0, 0, 0, 0, 0, 0}, //
      {0, 3, 0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 5, 0, 0}, //
      {0, 0, 0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 0, 0, 0}  //
  };

  const mln::image2d<std::int16_t> ref{
      {1, 1, 1, 1, 0, 2, 2, 2}, //
      {1, 1, 1, 1, 0, 2, 2, 2}, //
      {1, 1, 1, 1, 0, 2, 2, 2}, //
      {1, 1, 1, 1, 0, 0, 0, 0}, //
      {1, 1, 1, 1, 1, 1, 1, 1}, //
  };

  int  nlabel;
  auto out = mln::morpho::watershed_from_markers<std::int16_t>(input, mln::c8, seeds, nlabel);
  ASSERT_IMAGES_EQ_EXP(out, ref);
  ASSERT_EQ(nlabel, 2);
}

TEST(Morpho, watershed_partition_c4)
{
  const mln::image2d<std::uint8_t> grad{
      {0, 0, 0, 2, 0}, //
      {0, 0, 0, 1, 0}, //
      {3, 3, 3, 2, 3}, //
      {0, 0, 3, 0, 0}, //
      {0, 0, 2, 0, 0}  //
  };

  const mln::image2d<std::int16_t> ref{
      {1, 1, 1, 1, 2}, //
      {1, 1, 1, 1, 2}, //
      {1, 1, 1, 4, 2}, //
      {3, 3, 3, 4, 4}, //
      {3, 3, 3, 4, 4}  //
  };

  int  nlabel;
  auto out = mln::morpho::watershed<std::int16_t>(grad, mln::c4, nlabel, false);
  ASSERT_IMAGES_EQ_EXP(out, ref);
  ASSERT_EQ(nlabel, 4);
}

TEST(Morpho, watershed_partition_c8)
{
  const mln::image2d<std::uint8_t> grad{
      {0, 0, 0, 2, 0}, //
      {0, 0, 0, 2, 0}, //
      {3, 3, 3, 1, 3}, //
      {0, 0, 3, 0, 0}, //
      {0, 0, 2, 0, 0}  //
  };

  const mln::image2d<std::int16_t> ref{
      {1, 1, 1, 1, 2}, //
      {1, 1, 1, 1, 2}, //
      {1, 1, 1, 1, 2}, //
      {3, 3, 3, 4, 4}, //
      {3, 3, 3, 4, 4}  //
  };

  int  nlabel;
  auto out = mln::morpho::watershed<std::int16_t>(grad, mln::c8, nlabel, false);
  ASSERT_IMAGES_EQ_EXP(out, ref);
  ASSERT_EQ(nlabel, 4);
}