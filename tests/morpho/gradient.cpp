#include <mln/morpho/experimental/gradient.hpp>

#include <mln/core/algorithm/iota.hpp>
#include <mln/core/algorithm/paste.hpp>
#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/image/view/filter.hpp>
#include <mln/core/se/rect2d.hpp>
#include <mln/core/colors.hpp>

#include <functional>

#include <fixtures/ImagePath/image_path.hpp>
#include <fixtures/ImageCompare/image_compare.hpp>


#include <mln/io/experimental/imread.hpp>
#include <mln/io/experimental/imprint.hpp>
#include <gtest/gtest.h>


using namespace mln;

TEST(Morpho, gradient_binary)
{
  mln::experimental::image2d<bool> ima = {{0, 0, 0, 0, 1, 0, 0},  //
                                          {0, 0, 0, 0, 1, 0, 0},  //
                                          {0, 1, 1, 1, 1, 1, 0},  //
                                          {0, 1, 1, 1, 1, 1, 1},  //
                                          {0, 1, 1, 1, 1, 1, 0}}; //


  mln::experimental::se::rect2d win(3, 3);


  {
    mln::experimental::image2d<bool> ref = {{0, 0, 0, 1, 1, 1, 0},  //
                                            {1, 1, 1, 1, 1, 1, 1},  //
                                            {1, 1, 1, 1, 1, 1, 1},  //
                                            {1, 1, 0, 0, 0, 1, 1},  //
                                            {1, 1, 0, 0, 0, 1, 1}}; //

    auto out = mln::morpho::experimental::gradient(ima, win);
    ASSERT_IMAGES_EQ_EXP(out, ref);
  }

  {
    mln::experimental::image2d<bool> ref = {{0, 0, 0, 1, 0, 1, 0},  //
                                            {1, 1, 1, 1, 0, 1, 1},  //
                                            {1, 0, 0, 0, 0, 0, 1},  //
                                            {1, 0, 0, 0, 0, 0, 0},  //
                                            {1, 0, 0, 0, 0, 0, 1}}; //


    auto out = mln::morpho::experimental::external_gradient(ima, win);
    ASSERT_IMAGES_EQ_EXP(out, ref);
  }

  {
  mln::experimental::image2d<bool> ref = {{0, 0, 0, 0, 1, 0, 0},  //
                                          {0, 0, 0, 0, 1, 0, 0},  //
                                          {0, 1, 1, 1, 1, 1, 0},  //
                                          {0, 1, 0, 0, 0, 1, 1},  //
                                          {0, 1, 0, 0, 0, 1, 0}}; //


    auto out = mln::morpho::experimental::internal_gradient(ima, win);
    ASSERT_IMAGES_EQ_EXP(out, ref);
  }

}

TEST(Morpho, gradient_grayscale)
{
  mln::experimental::image2d<uint8_t> ima = {{0, 0, 0, 0, 9, 0, 0},  //
                                             {0, 0, 0, 0, 9, 0, 0},  //
                                             {0, 9, 9, 9, 9, 9, 0},  //
                                             {0, 9, 9, 9, 9, 9, 9},  //
                                             {0, 9, 9, 9, 9, 9, 0}}; //


  mln::experimental::se::rect2d win(3, 3);


  {
    mln::experimental::image2d<uint8_t> ref = {{0, 0, 0, 9, 9, 9, 0},  //
                                               {9, 9, 9, 9, 9, 9, 9},  //
                                               {9, 9, 9, 9, 9, 9, 9},  //
                                               {9, 9, 0, 0, 0, 9, 9},  //
                                               {9, 9, 0, 0, 0, 9, 9}}; //

    auto out = mln::morpho::experimental::gradient(ima, win);
    ASSERT_IMAGES_EQ_EXP(out, ref);
  }

  {
    mln::experimental::image2d<uint8_t> ref = {{0, 0, 0, 9, 0, 9, 0},  //
                                               {9, 9, 9, 9, 0, 9, 9},  //
                                               {9, 0, 0, 0, 0, 0, 9},  //
                                               {9, 0, 0, 0, 0, 0, 0},  //
                                               {9, 0, 0, 0, 0, 0, 9}}; //


    auto out = mln::morpho::experimental::external_gradient(ima, win);
    ASSERT_IMAGES_EQ_EXP(out, ref);
  }

  {
    mln::experimental::image2d<uint8_t> ref = {{0, 0, 0, 0, 9, 0, 0},  //
                                               {0, 0, 0, 0, 9, 0, 0},  //
                                               {0, 9, 9, 9, 9, 9, 0},  //
                                               {0, 9, 0, 0, 0, 9, 9},  //
                                               {0, 9, 0, 0, 0, 9, 0}}; //


    auto out = mln::morpho::experimental::internal_gradient(ima, win);
    ASSERT_IMAGES_EQ_EXP(out, ref);
  }
}


// This test is disabled because of a lifetime problem on the filter view (the domain lifetime depends on the image which might be a temporary).
TEST(Morpho, DISABLED_gradient_on_view)
{
  using namespace mln::view::ops;
  mln::experimental::image2d<uint8> ima(10, 5);
  mln::iota(ima, 0);


  // Morpher has no extension
  mln::experimental::se::rect2d win(3, 3);

  constexpr uint8 _X = 0;
  mln::experimental::image2d<uint8_t> ref = {{_X, _X, _X, _X, _X, _X, _X, _X, _X, _X},
                                             {_X, _X, _X, _X, _X, _X, _X, _X, _X, _X},
                                             {_X, _X, _X, _X, _X, _X, 11, 12, 12, 11},
                                             {11, 12, 12, 12, 12, 20, 21, 22, 22, 21},
                                             {11, 12, 12, 12, 12, 12, 12, 12, 12, 11}};

  auto gt_than = [](uint8_t x) { return std::bind(std::greater<uint8_t>(), std::placeholders::_1, x); };

  auto tmp = mln::view::filter(ima, gt_than(25));
  auto out = mln::morpho::experimental::gradient(tmp, win);

  // Does not compile because domain differs
  //ASSERT_IMAGES_EQ_EXP(mln::view::filter(ref, gt_than(0)), out);

  mln::image_build_params init_params;
  init_params.init_value = _X;
  mln::experimental::image2d<uint8> out2(10, 5, init_params);

  //mln::paste(out, out2);
  //mln::io::experimental::imprint(out);
  ASSERT_IMAGES_EQ_EXP(out2, ref);
}


// On colors
TEST(Morpho, gradient_color)
{
  using namespace mln::view::ops;

  mln::experimental::image2d<mln::rgb8> ima;
  mln::io::experimental::imread(fixtures::ImagePath::concat_with_filename("small.ppm"), ima);

  mln::experimental::se::rect2d win(3, 3);
  auto grad1 = mln::morpho::experimental::gradient(ima, win);
  auto grad2 = mln::morpho::experimental::internal_gradient(ima, win);
  auto grad3 = mln::morpho::experimental::external_gradient(ima, win);

  ASSERT_TRUE(mln::all_of(grad2 <= grad1));
  ASSERT_TRUE(mln::all_of(grad3 <= grad1));
}

