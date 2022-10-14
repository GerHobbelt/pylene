#include <mln/morpho/dilation.hpp>

#include <mln/core/colors.hpp>
#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/algorithm/clone.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/clip.hpp>
#include <mln/core/image/view/mask.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/image/view/rgb.hpp>
#include <mln/core/image/view/cast.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/core/se/rect2d.hpp>
#include <mln/io/imread.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>
#include <fixtures/ImagePath/image_path.hpp>

#include <gtest/gtest.h>
#include <tbb/task_scheduler_init.h>


using namespace mln;

void test_dilation_by_periodic_line(const mln::point2d& dp, int k)
{
  int kWidth = 9;
  int kHeight = 5;

  assert((dp >= mln::point2d{0, 0}));

  mln::image_build_params params;
  params.init_value = uint8_t(0);
  params.border     = 4;

  mln::image2d<uint8_t> input(kWidth, kHeight, params);
  mln::iota(input, 0);

  // Generate ref
  mln::image2d<uint8_t> ref = mln::clone(input);
  ref.extension().fill(0);

  for (int i = 0; i < k; ++i)
    mln_foreach (auto p, ref.domain())
      ref(p) = std::max(ref(p), ref.at(p + dp));

  // Run algo
  auto line = mln::se::periodic_line2d(dp, k);
  mln::morpho::dilation(input, line, input);
  ASSERT_IMAGES_EQ_EXP(ref, input);
}



TEST(Dilation, PeriodicLine2d_horizontal)
{
  test_dilation_by_periodic_line(mln::point2d{1, 0}, 3);
}

TEST(Dilation, PeriodicLine2d_vertical)
{
  test_dilation_by_periodic_line(mln::point2d{0, 1}, 3);
}

TEST(Dilation, PeriodicLine2d_diagonal)
{
  test_dilation_by_periodic_line(mln::point2d{-1, 1}, 2);
}

TEST(Dilation, PeriodicLine2d_horizontal_knightmove)
{
  test_dilation_by_periodic_line(mln::point2d{-2, 1}, 2);
}

TEST(Dilation, PeriodicLine2d_vertical_knightmove)
{
  test_dilation_by_periodic_line(mln::point2d{-1, 2}, 2);
}



TEST(Dilation, Rectangle2d_with_side_effects)
{
  mln::image_build_params params;
  params.init_value = uint8_t(0);

  mln::image2d<uint8_t> input(7, 7, params);
  input({3, 3}) = 1;
  input.at({-1, -1}) = 2;
  input.at({-1, +7}) = 2;
  input.at({+7, +7}) = 2;
  input.at({+7, -1}) = 2;

  mln::image2d<uint8_t> ref = {
    {2, 2, 0, 0, 0, 2, 2}, //
    {2, 2, 1, 1, 1, 2, 2}, //
    {0, 1, 1, 1, 1, 1, 0}, //
    {0, 1, 1, 1, 1, 1, 0}, //
    {0, 1, 1, 1, 1, 1, 0}, //
    {2, 2, 1, 1, 1, 2, 2}, //
    {2, 2, 0, 0, 0, 2, 2}, //
  };

  mln::image2d<uint8_t> output(7, 7);
  input.inflate_domain_to_extension();

  mln::morpho::dilation(input, mln::se::rect2d(5, 5), mln::extension::bm::user{}, output);
  ASSERT_IMAGES_EQ_EXP(ref, output);
}



TEST(Dilation, Generic_with_wide_enough_extension)
{
  using namespace mln::view::ops;

  mln::image2d<uint8_t> ima;
  mln::io::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);

  { // Fast: border wide enough
    auto se  = mln::se::disc(3, mln::se::disc::EXACT);
    auto out = mln::morpho::dilation(ima, se);
    ASSERT_TRUE(mln::all_of(out >= ima)); // extensive
  }
}


// Border is not wide enough => use morpher for bound checking
TEST(Dilation, Generic_with_too_small_extension)
{
  using namespace mln::view::ops;

  mln::image2d<uint8_t> ima;
  mln::io::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);

  {
    auto se  = mln::se::disc(4, mln::se::disc::EXACT);
    auto out = mln::morpho::dilation(ima, se);
    ASSERT_TRUE(mln::all_of(out >= ima)); // extensive
  }
}

// Dilation on a with a vmorph / binary case
TEST(Dilation, Square_on_a_vmorph)
{
  using namespace mln::view::ops;

  mln::image2d<uint8_t> ima;
  mln::io::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);

  auto input = ima > 128;
  auto win = mln::se::rect2d(3, 3);
  auto out = mln::morpho::dilation(input, win);
  ASSERT_TRUE(mln::all_of(out >= input)); // extensive
}


TEST(Dilation, Unregular_domain)
{
  using namespace mln::view::ops;

  mln::image2d<std::uint8_t> ima;
  mln::io::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);

  auto win = mln::se::rect2d(3, 3);
  auto out = mln::clone(ima);
  auto dom = mln::where(ima > 128);


  mln::morpho::dilation(mln::view::clip(ima, dom), win, mln::view::clip(out, dom));
  ASSERT_TRUE(mln::all_of(mln::view::mask(out, ima <= 128) == mln::view::mask(ima, ima <= 128)));
  ASSERT_TRUE(mln::all_of(out >= ima)); // extensive
}

/*
// Not available (need projector / value_set)

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
  mln::image2d<bool> ima(11, 11);
  mln::fill(ima, false);
  ima({0, 0})   = true;
  ima({5, 5})   = true;
  ima({10, 10}) = true;

  mln::image2d<bool> ref = {
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

  auto win = mln::se::rect2d(3, 3);
  auto out = mln::morpho::dilation(ima, win);
  ASSERT_IMAGES_EQ_EXP(ref, out);
}

// Dilation of a bianry image
TEST(Dilation, Binary_2)
{
  using namespace mln::view::ops;

  mln::image2d<bool> ima;
  mln::io::imread(fixtures::ImagePath::concat_with_filename("tiny.pbm"), ima);

  auto win = mln::se::rect2d(3, 3);
  auto out = mln::morpho::dilation(ima, win);
  ASSERT_TRUE(all_of(ima <= out)); // anti-extensive
}

// Dilation of a rgb image
TEST(Dilation, RGB)
{
  using namespace mln::view::ops;

  mln::image2d<mln::rgb8> ima;
  mln::io::imread(fixtures::ImagePath::concat_with_filename("small.ppm"), ima);

  auto win = mln::se::rect2d(5, 5);
  auto out = mln::morpho::dilation(ima, win);

  ASSERT_TRUE(mln::all_of(mln::view::red(ima) <= mln::view::red(out)));     // anti-extensive
  ASSERT_TRUE(mln::all_of(mln::view::green(ima) <= mln::view::green(out))); // anti-extensive
  ASSERT_TRUE(mln::all_of(mln::view::blue(ima) <= mln::view::blue(out)));   // anti-extensive
}


enum e_se { RECTANGLE, DISC, DISC_APPROX };
enum e_run { SEQUENTIAL, PARALLEL };

struct test_param_t
{
  e_se  se;
  e_run parallel;
  int   tile_width  = 8;
  int   tile_height = 8;
};


mln::image2d<uint8_t> ref[3] = {
    // RECTANGLE
    mln::image2d<uint8_t>{
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
    },
    // DISC
    mln::image2d<uint8_t>{
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
        {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0}, //
        {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0}, //
        {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0}, //
        {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0}, //
        {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0}, //
        {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0}, //
        {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0}, //
        {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0}, //
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}, //
        {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0}, //
        {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0}, //
        {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0}, //
        {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0}, //
        {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0}, //
        {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0}, //
        {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0}, //
        {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0}, //
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //
    },
    // DISC_APPROX
    mln::image2d<uint8_t>{
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
    }};



class DilationTestSuite : public testing::TestWithParam<test_param_t>
{
public:
  template <class V>
  void test_with(test_param_t tparams);

private:
  tbb::task_scheduler_init __tinit = {};
};

template <class V>
void DilationTestSuite::test_with(test_param_t tparam)
{
    mln::image_build_params params;
    params.init_value = V(0);

    mln::image2d<V> input(21, 21, params);
    input({10, 10}) = 1;

    mln::image2d<V> output;

    if (tparam.parallel == PARALLEL)
    {
      switch (tparam.se)
      {
      case RECTANGLE:
        output = mln::morpho::parallel::dilation(input, mln::se::rect2d(19, 15), tparam.tile_width, tparam.tile_height);
        break;
      case DISC:
        output = mln::morpho::parallel::dilation(input, mln::se::disc(9, mln::se::disc::EXACT), tparam.tile_width,
                                                 tparam.tile_height);
        break;
      case DISC_APPROX:
        output = mln::morpho::parallel::dilation(input, mln::se::disc(9), tparam.tile_width, tparam.tile_height);
        break;
      }
  }
  else
  {
    switch (tparam.se)
    {
    case RECTANGLE:
      output = mln::morpho::dilation(input, mln::se::rect2d(19, 15));
      break;
    case DISC:
      output = mln::morpho::dilation(input, mln::se::disc(9, mln::se::disc::EXACT));
      break;
    case DISC_APPROX:
      output = mln::morpho::dilation(input, mln::se::disc(9));
      break;
    }
  }

    if constexpr (std::is_same_v<V, uint8_t>)
      ASSERT_IMAGES_EQ_EXP(ref[tparam.se], output);
    else
      ASSERT_IMAGES_EQ_EXP(mln::view::cast<V>(ref[tparam.se]), output);
}


TEST_P(DilationTestSuite, Exec)
{
  test_param_t tparam = GetParam();
  test_with<uint8_t>(tparam);
  test_with<int8_t>(tparam);
}


std::vector<test_param_t> test_suite_params = {
    {RECTANGLE, SEQUENTIAL, 0, 0},   //
    {DISC, SEQUENTIAL, 0, 0},        //
    {DISC_APPROX, SEQUENTIAL, 0, 0}, //
    {RECTANGLE, PARALLEL, 8, 8},     //
    {DISC, PARALLEL, 8, 8},          //
    {DISC_APPROX, PARALLEL, 8, 8},   //
    {RECTANGLE, PARALLEL, 4, 4},     //
    {DISC, PARALLEL, 4, 4},          //
    {DISC_APPROX, PARALLEL, 4, 4},   //
    {RECTANGLE, PARALLEL, 3, 3},     //
    {DISC, PARALLEL, 3, 3},          //
    {DISC_APPROX, PARALLEL, 3, 3},   //
    {RECTANGLE, PARALLEL, 2, 2},     //
    {DISC, PARALLEL, 2, 2},          //
    {DISC_APPROX, PARALLEL, 2, 2},   //
    {RECTANGLE, PARALLEL, 3, 5},     //
    {DISC, PARALLEL, 3, 5},          //
    {DISC_APPROX, PARALLEL, 3, 5},   //
};


std::string print_test_name(const testing::TestParamInfo<test_param_t>& info)
{
  static const char* fmt_se_name[3] = {"Rectangle", "Disc", "Disc_Approximated"};
  static const char* fmt_mode_name[2] = { "Sequential", "Parallel"};

  return fmt::format("{}_{}_{}_{}", fmt_se_name[info.param.se], fmt_mode_name[info.param.parallel], info.param.tile_width, info.param.tile_height);
}

INSTANTIATE_TEST_SUITE_P(Morpho, DilationTestSuite, testing::ValuesIn(test_suite_params), print_test_name);

