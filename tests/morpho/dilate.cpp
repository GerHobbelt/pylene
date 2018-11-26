#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/win2d.hpp>
#include <mln/io/imread.hpp>
#include <mln/morpho/canvas/private/dilation_by_periodic_line.hpp>
#include <mln/morpho/structural/dilate.hpp>

#include <mln/core/se/disc.hpp>
#include <mln/core/se/rect2d.hpp>

#include <helpers.hpp>

#include <gtest/gtest.h>

using namespace mln;
#define MLN_IMG_PATH "../../img/"

void test_dilation_by_periodic_line(const mln::point2d& dp, int k)
{
  mln_assertion((dp >= mln::point2d{0, 0}));

  mln::image2d<mln::uint8> input(5, 9);
  mln::iota(input, 0);

  // Generate ref
  mln::image2d<mln::uint8> ref = mln::clone(input);
  for (int i = 0; i < k; ++i)
    mln_foreach (auto p, ref.domain())
      if (ref.domain().has(p + dp))
        ref(p) = ref(p + dp);

  // Run algo
  mln::se::periodic_line2d line(dp, k);
  auto                     sup = [](auto x, auto y) { return std::max(x, y); };
  mln::morpho::internal::dilation_by_periodic_line(input, line, sup, 0);
  ASSERT_IMAGES_EQ(ref, input);
}


TEST(Dilation, PeriodicLine2d_horizontal)
{
  test_dilation_by_periodic_line(mln::point2d{0, 1}, 3);
}

TEST(Dilation, PeriodicLine2d_vertical)
{
  test_dilation_by_periodic_line(mln::point2d{1, 0}, 3);
}

TEST(Dilation, PeriodicLine2d_diagonal)
{
  test_dilation_by_periodic_line(mln::point2d{1, -1}, 2);
}

TEST(Dilation, PeriodicLine2d_horizontal_knightmove)
{
  test_dilation_by_periodic_line(mln::point2d{1, -2}, 2);
}

TEST(Dilation, PeriodicLine2d_vertical_knightmove)
{
  test_dilation_by_periodic_line(mln::point2d{2, -1}, 2);
}


TEST(Dilation, Disc_approximated)
{
  mln::box2d               domain{{0, 0}, {21, 21}};
  mln::image2d<mln::uint8> input(domain, 0);

  const mln::image2d<mln::uint8> ref = {
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0}, //
      {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0}, //
      {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0}, //
      {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0}, //
      {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0}, //
      {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}, //
      {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}, //
      {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}, //
      {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0}, //
      {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0}, //
      {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0}, //
      {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0}, //
      {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
  };

  input.at(10, 10) = 1;
  auto output      = mln::morpho::structural::dilate(input, mln::se::disc(9));
  ASSERT_IMAGES_EQ(ref, output);
}

TEST(Dilation, Disc_euclidean)
{
  mln::box2d               domain{{0, 0}, {21, 21}};
  mln::image2d<mln::uint8> input(domain, 0);

  const mln::image2d<mln::uint8> ref = {
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
      {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
      {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
      {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
      {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
      {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
      {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
      {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
      {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
      {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
      {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
      {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
      {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  };

  input.at(10, 10) = 1;
  auto output      = mln::morpho::structural::dilate(input, mln::se::disc(9, 0));
  ASSERT_IMAGES_EQ(ref, output);
}

TEST(Dilation, Rectangle2d)
{
  mln::box2d               domain{{0, 0}, {21, 21}};
  mln::image2d<mln::uint8> input(domain, 0);
  input.at(10, 10) = 1;

  const mln::image2d<mln::uint8> ref = {
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
      {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}, //
      {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}, //
      {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}, //
      {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}, //
      {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}, //
      {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}, //
      {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}, //
      {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}, //
      {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}, //
      {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}, //
      {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}, //
      {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}, //
      {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}, //
      {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}, //
      {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}, //
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}  //
  };

  auto output = mln::morpho::structural::dilate(input, mln::se::rect2d(19, 15));
  ASSERT_IMAGES_EQ(ref, output);
}

TEST(Dilation, Generic_with_wide_enough_extension)
{
  image2d<uint8> ima;
  io::imread(MLN_IMG_PATH "small.pgm", ima);

  { // Fast: border wide enough
    mln::se::disc se(3, 0);
    auto          out = morpho::structural::dilate(ima, se);
    ASSERT_TRUE(all(out >= ima)); // extensive
  }
}

// Border is not wide enough => use morpher for bound checking
TEST(Dilation, Generic_with_too_small_extension)
{
  image2d<uint8> ima;
  io::imread(MLN_IMG_PATH "small.pgm", ima);

  mln::se::disc se(4, 0);
  auto          out = morpho::structural::dilate(ima, se);
  ASSERT_TRUE(all(out >= ima)); // extensive
}

// Dilation on a with a vmorph / binary case
TEST(Dilation, Square_on_a_vmorph)
{
  image2d<uint8> ima;
  io::imread(MLN_IMG_PATH "small.pgm", ima);

  mln::se::rect2d win(3, 3);
  auto            out = morpho::structural::dilate(ima > 128, win);
  ASSERT_TRUE(all(out >= (ima > 128))); // extensive
}

TEST(Dilation, Unregular_domain)
{
  image2d<uint8> ima;
  io::imread(MLN_IMG_PATH "small.pgm", ima);

  mln::se::rect2d win(3, 3);
  auto            out = clone(ima);
  auto            tmp = out | where(ima > 128);
  morpho::structural::dilate(ima | where(ima > 128), win, tmp, std::less<uint8>());
  ASSERT_TRUE(all((out | (ima <= 128)) == (ima | (ima <= 128))));
  ASSERT_TRUE(all(out >= ima)); // extensive
}

// Custom comparison function, erosion
TEST(Dilation, Custom_cmp_function)
{
  image2d<uint8> ima;
  io::imread(MLN_IMG_PATH "small.pgm", ima);

  mln::se::rect2d win(5, 5);
  auto            out = morpho::structural::dilate(ima, win, std::greater<uint8>());
  ASSERT_TRUE(all(out <= ima)); // anti-extensive
}

// Dilation of a binary image
TEST(Dilation, Binary)
{
  image2d<bool> ima(11, 11);
  fill(ima, false);
  ima.at(0, 0)   = true;
  ima.at(5, 5)   = true;
  ima.at(10, 10) = true;

  image2d<bool> ref = {
      {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
      {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1}, //
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1}  //
  };

  mln::se::rect2d win(3, 3);
  auto            out = morpho::structural::dilate(ima, win);
  ASSERT_IMAGES_EQ(ref, out);
}

// Dilation of a bianry image
TEST(Dilation, Binary_2)
{
  image2d<bool> ima;
  io::imread(MLN_IMG_PATH "tiny.pbm", ima);

  mln::se::rect2d win(3, 3);
  auto            out = morpho::structural::dilate(ima, win);
  ASSERT_TRUE(all(ima <= out)); // anti-extensive
}

// Dilation of a rgb image
TEST(Dilation, RGB)
{
  image2d<rgb8> ima;
  io::imread(MLN_IMG_PATH "small.ppm", ima);

  mln::se::rect2d win(5, 5);
  auto            out = morpho::structural::dilate(ima, win);
  ASSERT_TRUE(all(red(ima) <= red(out)));     // anti-extensive
  ASSERT_TRUE(all(green(ima) <= green(out))); // anti-extensive
  ASSERT_TRUE(all(blue(ima) <= blue(out)));   // anti-extensive
}
