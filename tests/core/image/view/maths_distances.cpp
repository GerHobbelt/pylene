#include <mln/core/image/view/maths.hpp>

#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/image/view/transform.hpp>

#include <gtest/gtest.h>
#include <fixtures/ImageCompare/image_compare.hpp>


TEST(View, maths_l0dist)
{
  using namespace mln::view::ops;
  using namespace mln::view::maths;

  mln::image2d<int> ima1 = {{0, -1, 2, -3, 4},  //
                                          {-5, 6, -4, 8, -9}, //
                                          {10, -11, 12, -13, 14}};

  mln::image2d<int> ima2 = {{0, -1, 2, -3, 4},  //
                                          {-5, 6, -4, 8, -9}, //
                                          {10, -11, 12, -13, 14}};

  mln::image2d<int> ref = {{0, 2, 2, 12, 12},    //
                                         {30, 30, 20, 56, 90}, //
                                         {90, 132, 132, 182, 182}};

  ASSERT_IMAGES_EQ_EXP(ref, l0dist(ima1, sqr(ima2)));



  mln::image2d<mln::rgb8> ima3 = {{{1, 2, 2}, {2, 4, 4}, {3, 6, 6}}, //
                                                {{1, 2, 2}, {2, 4, 4}, {3, 6, 6}}, //
                                                {{1, 2, 2}, {2, 4, 4}, {3, 6, 6}}};

  mln::image2d<mln::rgb8> ima4 = {{{3, 6, 6}, {2, 4, 4}, {1, 2, 2}}, //
                                                {{3, 6, 6}, {2, 4, 4}, {1, 2, 2}}, //
                                                {{3, 6, 6}, {2, 4, 4}, {1, 2, 2}}};

  mln::image2d<int> ref2 = {{2, 0, 2}, //
                                          {2, 0, 2}, //
                                          {2, 0, 2}};

  ASSERT_IMAGES_EQ_EXP(ref2, l0dist(ima3, ima4));
}

TEST(View, maths_l1dist)
{
  using namespace mln::view::ops;
  using namespace mln::view::maths;

  mln::image2d<int> ima1 = {{0, -1, 2, -3, 4},  //
                                          {-5, 6, -4, 8, -9}, //
                                          {10, -11, 12, -13, 14}};

  mln::image2d<int> ima2 = {{0, -1, 2, -3, 4},  //
                                          {-5, 6, -4, 8, -9}, //
                                          {10, -11, 12, -13, 14}};

  mln::image2d<int> ref = {{0, 2, 2, 12, 12},    //
                                         {30, 30, 20, 56, 90}, //
                                         {90, 132, 132, 182, 182}};

  ASSERT_IMAGES_EQ_EXP(ref, l1dist(ima1, sqr(ima2)));


  mln::image2d<mln::rgb8> ima3 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  mln::image2d<mln::rgb8> ima4 = {{{7, 8, 9}, {4, 5, 6}, {1, 2, 3}}, //
                                                {{7, 8, 9}, {4, 5, 6}, {1, 2, 3}}, //
                                                {{7, 8, 9}, {4, 5, 6}, {1, 2, 3}}};

  mln::image2d<int> ref2 = {{18, 0, 18}, //
                                          {18, 0, 18}, //
                                          {18, 0, 18}};

  ASSERT_IMAGES_EQ_EXP(ref2, l1dist(ima3, ima4));
}

TEST(View, maths_l2dist)
{
  using namespace mln::view::ops;
  using namespace mln::view::maths;

  mln::image2d<int> ima1 = {{0, -1, 2, -3, 4},  //
                                          {-5, 6, -4, 8, -9}, //
                                          {10, -11, 12, -13, 14}};

  mln::image2d<int> ima2 = {{0, -1, 2, -3, 4},  //
                                          {-5, 6, -4, 8, -9}, //
                                          {10, -11, 12, -13, 14}};

  mln::image2d<double> ref = {{0, 2, 2, 12, 12},    //
                                            {30, 30, 20, 56, 90}, //
                                            {90, 132, 132, 182, 182}};

  ASSERT_IMAGES_EQ_EXP(ref, l2dist(ima1, sqr(ima2)));


  mln::image2d<mln::rgb8> ima3 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  mln::image2d<mln::rgb8> ima4 = {{{5, 6, 5}, {4, 5, 6}, {5, 4, 5}}, //
                                                {{5, 6, 5}, {4, 5, 6}, {5, 4, 5}}, //
                                                {{5, 6, 5}, {4, 5, 6}, {5, 4, 5}}};

  mln::image2d<double> ref2 = {{6, 0, 6}, //
                                             {6, 0, 6}, //
                                             {6, 0, 6}};

  ASSERT_IMAGES_EQ_EXP(ref2, l2dist(ima3, ima4));
}

TEST(View, maths_lpdist)
{
  using namespace mln::view::ops;
  using namespace mln::view::maths;

  mln::image2d<int> ima1 = {{0, -1, 2, -3, 4},  //
                                          {-5, 6, -4, 8, -9}, //
                                          {10, -11, 12, -13, 14}};

  mln::image2d<int> ima2 = {{0, -1, 2, -3, 4},  //
                                          {-5, 6, -4, 8, -9}, //
                                          {10, -11, 12, -13, 14}};

  mln::image2d<int> ref = {{0, 2, 2, 12, 12},    //
                                         {30, 30, 20, 56, 90}, //
                                         {90, 132, 132, 182, 182}};

  ASSERT_IMAGES_EQ_EXP(ref , lpdist<4>(ima1, sqr(ima2)));


  mln::image2d<mln::rgb8> ima3 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  mln::image2d<mln::rgb8> ima4 = {{{7, 8, 9}, {4, 5, 6}, {1, 2, 3}}, //
                                                {{7, 8, 9}, {4, 5, 6}, {1, 2, 3}}, //
                                                {{7, 8, 9}, {4, 5, 6}, {1, 2, 3}}};

  mln::image2d<double> ref2 = {
      {std::pow(6 * 6 * 6 * 6 * 3, 1. / 4.), 0, std::pow(6 * 6 * 6 * 6 * 3, 1. / 4.)}, //
      {std::pow(6 * 6 * 6 * 6 * 3, 1. / 4.), 0, std::pow(6 * 6 * 6 * 6 * 3, 1. / 4.)}, //
      {std::pow(6 * 6 * 6 * 6 * 3, 1. / 4.), 0, std::pow(6 * 6 * 6 * 6 * 3, 1. / 4.)}};

  ASSERT_IMAGES_EQ_EXP(ref2,  lpdist<4>(ima3, ima4));
}

TEST(View, maths_l2dist_sqr)
{
  using namespace mln::view::ops;
  using namespace mln::view::maths;

  mln::image2d<int> ima1 = {{0, -1, 2, -3, 4},  //
                                          {-5, 6, -4, 8, -9}, //
                                          {10, -11, 12, -13, 14}};

  mln::image2d<int> ima2 = {{0, -1, 2, -3, 4},  //
                                          {-5, 6, -4, 8, -9}, //
                                          {10, -11, 12, -13, 14}};

  mln::image2d<int> ref = {{0, 4, 4, 144, 144},         //
                                         {900, 900, 400, 3136, 8100}, //
                                         {8100, 17424, 17424, 33124, 33124}};

  ASSERT_IMAGES_EQ_EXP(ref , l2dist_sqr(ima1, sqr(ima2)));


  mln::image2d<mln::rgb8> ima3 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  mln::image2d<mln::rgb8> ima4 = {{{7, 8, 9}, {4, 5, 6}, {1, 2, 3}}, //
                                                {{7, 8, 9}, {4, 5, 6}, {1, 2, 3}}, //
                                                {{7, 8, 9}, {4, 5, 6}, {1, 2, 3}}};

  mln::image2d<int> ref2 = {{108, 0, 108}, //
                                          {108, 0, 108}, //
                                          {108, 0, 108}};

ASSERT_IMAGES_EQ_EXP(ref2,  l2dist_sqr(ima3, ima4));
}

TEST(View, maths_linfdist)
{
  using namespace mln::view::ops;
  using namespace mln::view::maths;

  mln::image2d<int> ima1 = {{0, -1, 2, -3, 4},  //
                                          {-5, 6, -4, 8, -9}, //
                                          {10, -11, 12, -13, 14}};

  mln::image2d<int> ima2 = {{0, -1, 2, -3, 4},  //
                                          {-5, 6, -4, 8, -9}, //
                                          {10, -11, 12, -13, 14}};

  mln::image2d<int> ref = {{0, 4, 4, 144, 144},         //
                                         {900, 900, 400, 3136, 8100}, //
                                         {8100, 17424, 17424, 33124, 33124}};

  ASSERT_IMAGES_EQ_EXP(ref , linfdist(ima1, sqr(ima2)));


  mln::image2d<mln::rgb8> ima3 = {{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, //
                                                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

  mln::image2d<mln::rgb8> ima4 = {{{7, 8, 9}, {4, 5, 6}, {1, 2, 3}}, //
                                                {{7, 8, 9}, {4, 5, 6}, {1, 2, 3}}, //
                                                {{7, 8, 9}, {4, 5, 6}, {1, 2, 3}}};

  mln::image2d<int> ref2 = {{6, 0, 6}, //
                                          {6, 0, 6}, //
                                          {6, 0, 6}};

  ASSERT_IMAGES_EQ_EXP(ref2,  linfdist(ima3, ima4));
}
