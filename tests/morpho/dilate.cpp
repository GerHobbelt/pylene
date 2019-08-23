#include <mln/morpho/experimental/dilation.hpp>

#include <mln/core/colors.hpp>
#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/image/view/clip.hpp>
#include <mln/core/image/view/mask.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/image/view/rgb.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/core/se/rect2d.hpp>
#include <mln/io/experimental/imread.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>
#include <fixtures/ImagePath/image_path.hpp>

#include <gtest/gtest.h>


using namespace mln;

void test_dilation_by_periodic_line(const mln::experimental::point2d& dp, int k)
{
  int kWidth = 9;
  int kHeight = 5;

  assert((dp >= mln::experimental::point2d{0, 0}));

  mln::image_build_params params;
  params.init_value = uint8_t(0);
  params.border     = 4;

  mln::experimental::image2d<uint8_t> input(kWidth, kHeight, params);
  mln::iota(input, 0);

  // Generate ref
  mln::experimental::image2d<uint8_t> ref = mln::clone(input);
  ref.extension().fill(0);

  for (int i = 0; i < k; ++i)
    mln_foreach_new (auto p, ref.domain())
      ref(p) = std::max(ref(p), ref.at(p + dp));

  // Run algo
  auto line = mln::experimental::se::periodic_line2d(dp, k);
  auto sup  = [](auto x, auto y) { return std::max(x, y); };
  mln::morpho::details::dilation_by_periodic_line(input, input, line, sup, input.domain());
  ASSERT_IMAGES_EQ_EXP(ref, input);
}



TEST(Dilation, PeriodicLine2d_horizontal)
{
  test_dilation_by_periodic_line(mln::experimental::point2d{1, 0}, 3);
}

TEST(Dilation, PeriodicLine2d_vertical)
{
  test_dilation_by_periodic_line(mln::experimental::point2d{0, 1}, 3);
}

TEST(Dilation, PeriodicLine2d_diagonal)
{
  test_dilation_by_periodic_line(mln::experimental::point2d{-1, 1}, 2);
}

TEST(Dilation, PeriodicLine2d_horizontal_knightmove)
{
  test_dilation_by_periodic_line(mln::experimental::point2d{-2, 1}, 2);
}

TEST(Dilation, PeriodicLine2d_vertical_knightmove)
{
  test_dilation_by_periodic_line(mln::experimental::point2d{-1, 2}, 2);
}


TEST(Dilation, Disc_approximated)
{
  mln::experimental::box2d            domain(21, 21);

  mln::image_build_params params;
  params.init_value = uint8_t(0);
  mln::experimental::image2d<uint8_t> input(domain, params);

  const mln::experimental::image2d<uint8_t> ref = {
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

  input({10, 10}) = 1;
  auto output      = mln::morpho::experimental::dilation(input, mln::experimental::se::disc(9));
  ASSERT_IMAGES_EQ_EXP(ref, output);
}



TEST(Dilation, Disc_euclidean)
{
  mln::image_build_params params;
  params.init_value = uint8_t(0);

  mln::experimental::image2d<uint8_t> input(21, 21, params);

  mln::experimental::image2d<uint8_t> ref = {
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

  input({10, 10}) = 1;

  auto output      = mln::morpho::experimental::dilation(input, mln::experimental::se::disc(9, mln::experimental::se::disc::EXACT));
  ASSERT_IMAGES_EQ_EXP(ref, output);
}


TEST(Dilation, Rectangle2d)
{
  mln::image_build_params params;
  params.init_value = uint8_t(0);

  mln::experimental::image2d<uint8_t> input(21, 21, params);
  input({10, 10}) = 1;

  const mln::experimental::image2d<uint8_t> ref = {
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

  auto output = mln::morpho::experimental::dilation(input, mln::experimental::se::rect2d(19, 15));
  ASSERT_IMAGES_EQ_EXP(ref, output);
}

TEST(Dilation, Rectangle2d_with_side_effects)
{
  mln::image_build_params params;
  params.init_value = uint8_t(0);

  mln::experimental::image2d<uint8_t> input(7, 7, params);
  input({3, 3}) = 1;
  input.at({-1, -1}) = 2;
  input.at({-1, +7}) = 2;
  input.at({+7, +7}) = 2;
  input.at({+7, -1}) = 2;

  mln::experimental::image2d<uint8_t> ref = {
    {2, 2, 0, 0, 0, 2, 2}, //
    {2, 2, 1, 1, 1, 2, 2}, //
    {0, 1, 1, 1, 1, 1, 0}, //
    {0, 1, 1, 1, 1, 1, 0}, //
    {0, 1, 1, 1, 1, 1, 0}, //
    {2, 2, 1, 1, 1, 2, 2}, //
    {2, 2, 0, 0, 0, 2, 2}, //
  };

  auto output = mln::morpho::experimental::dilation(input, mln::experimental::se::rect2d(5, 5), mln::extension::bm::user{});
  ASSERT_IMAGES_EQ_EXP(ref, output);
}



TEST(Dilation, Generic_with_wide_enough_extension)
{
  using namespace mln::view::ops;

  mln::experimental::image2d<uint8_t> ima;
  mln::io::experimental::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);

  { // Fast: border wide enough
    auto se  = mln::experimental::se::disc(3, mln::experimental::se::disc::EXACT);
    auto out = mln::morpho::experimental::dilation(ima, se);
    ASSERT_TRUE(mln::all_of(out >= ima)); // extensive
  }
}


// Border is not wide enough => use morpher for bound checking
TEST(Dilation, Generic_with_too_small_extension)
{
  using namespace mln::view::ops;

  mln::experimental::image2d<uint8_t> ima;
  mln::io::experimental::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);

  {
    auto se  = mln::experimental::se::disc(4, mln::experimental::se::disc::EXACT);
    auto out = mln::morpho::experimental::dilation(ima, se);
    ASSERT_TRUE(mln::all_of(out >= ima)); // extensive
  }
}

// Dilation on a with a vmorph / binary case
TEST(Dilation, Square_on_a_vmorph)
{
  using namespace mln::view::ops;

  mln::experimental::image2d<uint8_t> ima;
  mln::io::experimental::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);

  auto input = ima > 128;
  auto win = mln::experimental::se::rect2d(3, 3);
  auto out = mln::morpho::experimental::dilation(input, win);
  ASSERT_TRUE(mln::all_of(out >= input)); // extensive
}


TEST(Dilation, Unregular_domain)
{
  using namespace mln::view::ops;

  mln::experimental::image2d<uint8> ima;
  mln::io::experimental::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);

  auto win = mln::experimental::se::rect2d(3, 3);
  auto out = mln::clone(ima);
  auto dom = mln::experimental::where(ima > 128);


  mln::morpho::experimental::dilation(mln::view::clip(ima, dom), win, mln::view::clip(out, dom));
  ASSERT_TRUE(mln::all_of(mln::view::mask(out, ima <= 128) == mln::view::mask(ima, ima <= 128)));
  ASSERT_TRUE(mln::all_of(out >= ima)); // extensive
}

/* Not available (need projector / value_set)

// Custom comparison function, erosion
TEST(Dilation, Custom_cmp_function)
{
  using namespace mln::view::ops;

  image2d<uint8> ima;
  io::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);

  mln::se::rect2d win(5, 5);
  auto            out = morpho::structural::dilate(ima, win, std::greater<uint8>());
  ASSERT_TRUE(all_of(out <= ima)); // anti-extensive
}

*/


// Dilation of a binary image
TEST(Dilation, Binary)
{
  mln::experimental::image2d<bool> ima(11, 11);
  mln::fill(ima, false);
  ima({0, 0})   = true;
  ima({5, 5})   = true;
  ima({10, 10}) = true;

  mln::experimental::image2d<bool> ref = {
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

  auto win = mln::experimental::se::rect2d(3, 3);
  auto out = mln::morpho::experimental::dilation(ima, win);
  ASSERT_IMAGES_EQ_EXP(ref, out);
}

// Dilation of a bianry image
TEST(Dilation, Binary_2)
{
  using namespace mln::view::ops;

  mln::experimental::image2d<bool> ima;
  mln::io::experimental::imread(fixtures::ImagePath::concat_with_filename("tiny.pbm"), ima);

  auto win = mln::experimental::se::rect2d(3, 3);
  auto out = mln::morpho::experimental::dilation(ima, win);
  ASSERT_TRUE(all_of(ima <= out)); // anti-extensive
}

// Dilation of a rgb image
TEST(Dilation, RGB)
{
  using namespace mln::view::ops;

  mln::experimental::image2d<mln::rgb8> ima;
  mln::io::experimental::imread(fixtures::ImagePath::concat_with_filename("small.ppm"), ima);

  auto win = mln::experimental::se::rect2d(5, 5);
  auto out = mln::morpho::experimental::dilation(ima, win);

  ASSERT_TRUE(mln::all_of(mln::view::red(ima) <= mln::view::red(out)));     // anti-extensive
  ASSERT_TRUE(mln::all_of(mln::view::green(ima) <= mln::view::green(out))); // anti-extensive
  ASSERT_TRUE(mln::all_of(mln::view::blue(ima) <= mln::view::blue(out)));   // anti-extensive
}


