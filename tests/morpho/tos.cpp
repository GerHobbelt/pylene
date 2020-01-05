#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/morpho/experimental/private/immersion.hpp>

#include <mln/core/image/view/operators.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>
#include <gtest/gtest.h>


TEST(ToSImmersion, twodimensional)
{
  const mln::experimental::image2d<uint8_t> f = {{0, 1, 2}, //
                                                    {3, 0, 1}};


  const mln::experimental::image2d<uint8_t> ref_inf = {{0, 0, 1, 1, 2}, //
                                                          {0, 0, 0, 0, 1}, //
                                                          {3, 0, 0, 0, 1}};

  const mln::experimental::image2d<uint8_t> ref_sup = {{0, 1, 1, 2, 2}, //
                                                          {3, 3, 1, 2, 2}, //
                                                          {3, 3, 0, 1, 1}};


  auto [inf, sup] = mln::morpho::experimental::details::immersion(f);
  ASSERT_IMAGES_EQ_EXP(ref_inf, inf);
  ASSERT_IMAGES_EQ_EXP(ref_sup, sup);
}

TEST(ToSImmersion, twodimensional_generic)
{
  using namespace mln::view::ops;
  const mln::experimental::image2d<uint8_t> f = {{0, 1, 2}, //
                                                    {3, 0, 1}};
  auto fprime = f * uint8_t(1);


  const mln::experimental::image2d<uint8_t> ref_inf = {{0, 0, 1, 1, 2}, //
                                                          {0, 0, 0, 0, 1}, //
                                                          {3, 0, 0, 0, 1}};

  const mln::experimental::image2d<uint8_t> ref_sup = {{0, 1, 1, 2, 2}, //
                                                          {3, 3, 1, 2, 2}, //
                                                          {3, 3, 0, 1, 1}};


  auto [inf, sup] = mln::morpho::experimental::details::immersion(fprime);
  ASSERT_IMAGES_EQ_EXP(ref_inf, inf);
  ASSERT_IMAGES_EQ_EXP(ref_sup, sup);
}

TEST(ToSImmersion, threedimensional)
{
  const mln::experimental::image3d<uint8_t> f = {
      {{0, 1, 2}, {3, 0, 1}}, //
      {{2, 0, 4}, {0, 0, 2}},
  };


  const mln::experimental::image3d<uint8_t> ref_inf = {
      {{0, 0, 1, 1, 2},  //
       {0, 0, 0, 0, 1},  //
       {3, 0, 0, 0, 1}}, //
      {{0, 0, 0, 0, 2},  //
       {0, 0, 0, 0, 1},  //
       {0, 0, 0, 0, 1}}, //
      {{2, 0, 0, 0, 4},  //
       {0, 0, 0, 0, 2},  //
       {0, 0, 0, 0, 2}}, //
  };

  const mln::experimental::image3d<uint8_t> ref_sup = {
      {{0, 1, 1, 2, 2},  //
       {3, 3, 1, 2, 2},  //
       {3, 3, 0, 1, 1}}, //
      {{2, 2, 1, 4, 4},  //
       {3, 3, 1, 4, 4},  //
       {3, 3, 0, 2, 2}}, //
      {{2, 2, 0, 4, 4},  //
       {2, 2, 0, 4, 4},  //
       {0, 0, 0, 2, 2}}, //
  };

  auto [inf, sup] = mln::morpho::experimental::details::immersion(f);
  ASSERT_IMAGES_EQ_EXP(ref_inf, inf);
  ASSERT_IMAGES_EQ_EXP(ref_sup, sup);
}
