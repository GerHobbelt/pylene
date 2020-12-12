#include <mln/core/canvas/morpho_pipeline.hpp>
#include <mln/morpho/closing.hpp>
#include <mln/morpho/opening.hpp>

#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/se/mask2d.hpp>
#include <mln/core/se/rect2d.hpp>
#include <mln/io/imread.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>
#include <fixtures/ImagePath/image_path.hpp>

#include <gtest/gtest.h>


TEST(Morpho, opening_closing_opening_0)
{
  using namespace mln::view::ops;

  mln::image2d<uint8_t> ima;
  mln::io::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);

  auto win = mln::se::rect2d(3, 3);
  {
    auto out1 = mln::morpho::opening(ima, win);
    auto out2 = mln::morpho::closing(ima, win);

    ASSERT_TRUE(all_of(out1 <= ima)); // anti-extensive
    ASSERT_TRUE(all_of(out2 >= ima)); // extensive
  }
}

TEST(Morpho, opening_closing_opening_1)
{
  mln::image2d<uint8_t> ima;
  mln::io::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);

  auto comp = [](uint8_t x) -> uint8_t { return 255 - x; };
  auto win = mln::se::rect2d(3, 3);
  {
    auto out1 = mln::morpho::opening(mln::view::transform(ima, comp), win);
    auto out2 = mln::morpho::closing(ima, win);

    ASSERT_IMAGES_EQ_EXP(out1, mln::view::transform(out2, comp)); // duality
  }
}

TEST(Morpho, opening_cross_1)
{
  mln::image2d<uint8_t> f =
    {{1, 0, 1, 0}, //
     {1, 1, 1, 1}, //
     {1, 0, 1, 0}}; //

  // With no border
  mln::image2d<uint8_t> ref_0 =
    {{1, 0, 1, 0}, //
     {1, 1, 1, 1}, //
     {1, 0, 1, 0}}; //

  // With a border set to 0
  mln::image2d<uint8_t> ref_1 =
    {{0, 0, 1, 0}, //
     {0, 1, 1, 1}, //
     {0, 0, 1, 0}}; //


  mln::se::mask2d m = {{0,1,0}, {1, 1, 1}, {0, 1, 0}};

  auto g_0 = mln::morpho::opening(f, m);
  auto g_1 = mln::morpho::opening(f, m, mln::extension::bm::fill(uint8_t(0)));

  ASSERT_IMAGES_EQ_EXP(g_0, ref_0);
  ASSERT_IMAGES_EQ_EXP(g_1, ref_1);
}

// We currently have a bug with non-symetric SE
TEST(Morpho, DISABLED_opening_non_symetric)
{
  mln::image2d<uint8_t> f =
    {{1, 0, 1, 1}, //
     {1, 1, 1, 1}, //
     {1, 0, 1, 0}}; //

  // With no border
  mln::image2d<uint8_t> ref_0 =
    {{1, 0, 1, 1}, //
     {1, 1, 1, 1}, //
     {0, 0, 0, 0}}; //

  // With a border set to 0
  mln::image2d<uint8_t> ref_1 =
    {{1, 0, 1, 0}, //
     {1, 1, 1, 1}, //
     {0, 0, 0, 0}}; //


  mln::se::mask2d m = {{0, 1, 0}, {0, 1, 1}, {0, 0, 0}};

  auto g_0 = mln::morpho::opening(f, m);
  auto g_1 = mln::morpho::opening(f, m, mln::extension::bm::fill(uint8_t(0)));

  ASSERT_IMAGES_EQ_EXP(g_0, ref_0);
  ASSERT_IMAGES_EQ_EXP(g_1, ref_1);
}

TEST(Morpho, opening_parallel)
{
  mln::image2d<uint8_t> ima;
  mln::io::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);
  auto win = mln::se::rect2d(3, 3);

  auto ref = mln::morpho::opening(ima, win);
  auto out = mln::morpho::parallel::opening(ima, win);

  ASSERT_IMAGES_EQ_EXP(out, ref);
}

TEST(Morpho, opening_parallel_inplace)
{
  mln::image2d<uint8_t> ima;
  mln::image2d<uint8_t> out;
  mln::image2d<uint8_t> ref;
  mln::io::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);
  auto win = mln::se::rect2d(3, 3);

  mln::morpho::opening(ima, win, ref);
  mln::morpho::parallel::opening(ima, win, out);

  ASSERT_IMAGES_EQ_EXP(out, ref);
}

TEST(Morpho, opening_parallel_rgb)
{
  mln::image2d<mln::rgb8> ima;
  mln::io::imread(fixtures::ImagePath::concat_with_filename("small.ppm"), ima);
  auto win = mln::se::rect2d(3, 3);

  auto ref = mln::morpho::opening(ima, win);
  auto out = mln::morpho::parallel::opening(ima, win);

  ASSERT_IMAGES_EQ_EXP(out, ref);
}

TEST(Morpho, opening_parallel_rgb_inplace)
{
  mln::image2d<mln::rgb8> ima;
  mln::image2d<mln::rgb8> out;
  mln::image2d<mln::rgb8> ref;
  mln::io::imread(fixtures::ImagePath::concat_with_filename("small.ppm"), ima);
  auto win = mln::se::rect2d(3, 3);

  mln::morpho::opening(ima, win, ref);
  mln::morpho::parallel::opening(ima, win, out);

  ASSERT_IMAGES_EQ_EXP(out, ref);
}
