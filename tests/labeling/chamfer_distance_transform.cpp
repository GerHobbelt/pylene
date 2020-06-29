#include <mln/labeling/chamfer_distance_transform.hpp>



#include <mln/core/image/ndimage.hpp>

#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/se/mask2d.hpp>


#include <fixtures/ImageCompare/image_compare.hpp>

#include <gtest/gtest.h>

TEST(Transform, chamfer_distance_transform_chamfer_distance_transform_1)
{
  mln::image2d<bool> f = {
      {0, 0, 0, 0, 0}, //
      {0, 1, 1, 1, 0}, //
      {0, 1, 1, 1, 0}, //
      {0, 1, 1, 1, 0}, //
      {0, 0, 0, 0, 0}  //
  };

  mln::image2d<int> ref = {
      {0, 0, 0, 0, 0}, //
      {0, 1, 1, 1, 0}, //
      {0, 1, 2, 1, 0}, //
      {0, 1, 1, 1, 0}, //
      {0, 0, 0, 0, 0}  //
  };

  auto res = mln::labeling::chamfer_distance_transform(f, mln::c4);

  ASSERT_IMAGES_EQ_EXP(res, ref);
}

TEST(Transform, chamfer_distance_transform_chamfer_distance_transform_2)
{

  mln::image2d<bool> f = {
      {1, 0, 0, 0, 0, 1}, //
      {0, 1, 1, 1, 1, 0}, //
      {0, 1, 1, 1, 1, 0}, //
      {0, 1, 1, 1, 1, 0}, //
      {1, 0, 0, 0, 0, 1}  //
  };

  mln::image2d<int> ref = {
      {1, 0, 0, 0, 0, 1}, //
      {0, 1, 1, 1, 1, 0}, //
      {0, 1, 2, 2, 1, 0}, //
      {0, 1, 1, 1, 1, 0}, //
      {1, 0, 0, 0, 0, 1}  //
  };

  auto res = mln::labeling::chamfer_distance_transform(f, mln::c4);

  ASSERT_IMAGES_EQ_EXP(res, ref);
}

// Input and output are the same image
TEST(Transform, chamfer_distance_transform_chamfer_distance_transform_3)
{

  mln::image2d<int> f = {
      {1, 0, 0, 0, 0, 1}, //
      {0, 1, 1, 1, 1, 0}, //
      {0, 1, 1, 1, 1, 0}, //
      {0, 1, 1, 1, 1, 0}, //
      {1, 0, 0, 0, 0, 1}  //
  };

  mln::image2d<int> ref = {
      {1, 0, 0, 0, 0, 1}, //
      {0, 1, 1, 1, 1, 0}, //
      {0, 1, 2, 2, 1, 0}, //
      {0, 1, 1, 1, 1, 0}, //
      {1, 0, 0, 0, 0, 1}  //
  };

  f = mln::labeling::chamfer_distance_transform(f, mln::c4);

  ASSERT_IMAGES_EQ_EXP(f, ref);
}

TEST(Transform, chamfer_distance_transform_bg_is_zero_distance_transform)
{

  mln::image2d<int> f = {
      {1, 1, 1, 1, 1, 1, 1}, //
      {1, 1, 1, 1, 1, 1, 1}, //
      {1, 1, 1, 1, 1, 1, 1}, //
      {1, 1, 1, 1, 1, 1, 1}, //
      {1, 1, 1, 1, 1, 1, 1}, //
      {1, 1, 1, 1, 1, 1, 1}, //
      {1, 1, 1, 1, 1, 1, 1}  //
  };

  mln::image2d<int> ref = {
      {1, 1, 1, 1, 1, 1, 1}, //
      {1, 2, 2, 2, 2, 2, 1}, //
      {1, 2, 3, 3, 3, 2, 1}, //
      {1, 2, 3, 4, 3, 2, 1}, //
      {1, 2, 3, 3, 3, 2, 1}, //
      {1, 2, 2, 2, 2, 2, 1}, //
      {1, 1, 1, 1, 1, 1, 1}  //
  };

  auto res = mln::labeling::chamfer_distance_transform(f, mln::c4);

  ASSERT_IMAGES_EQ_EXP(res, ref);
}

TEST(Transform, chamfer_distance_transform_bg_is_one_distance_transform)
{

  mln::image2d<int> f = {
      {1, 1, 1, 1, 1, 1, 1}, //
      {1, 1, 1, 1, 1, 1, 1}, //
      {1, 1, 1, 1, 1, 1, 1}, //
      {1, 1, 1, 1, 1, 1, 1}, //
      {1, 1, 1, 1, 1, 1, 1}, //
      {1, 1, 1, 1, 1, 1, 1}, //
      {1, 1, 1, 1, 1, 1, 1}  //
  };

  int maxv = std::numeric_limits<int>::max();

  mln::image2d<int> ref = {
      {maxv, maxv, maxv, maxv, maxv, maxv, maxv}, //
      {maxv, maxv, maxv, maxv, maxv, maxv, maxv}, //
      {maxv, maxv, maxv, maxv, maxv, maxv, maxv}, //
      {maxv, maxv, maxv, maxv, maxv, maxv, maxv}, //
      {maxv, maxv, maxv, maxv, maxv, maxv, maxv}, //
      {maxv, maxv, maxv, maxv, maxv, maxv, maxv}, //
      {maxv, maxv, maxv, maxv, maxv, maxv, maxv}  //
  };

  auto res = mln::labeling::chamfer_distance_transform(f, mln::c4, true);

  ASSERT_IMAGES_EQ_EXP(res, ref);
}

TEST(Transform, chamfer_distance_transform_bg_is_zero_weighted_distance_transform_float)
{

  mln::image2d<int> f = {
      {1, 1, 1, 1, 1, 1, 1}, //
      {1, 1, 1, 1, 1, 1, 1}, //
      {1, 1, 1, 1, 1, 1, 1}, //
      {1, 1, 1, 0, 1, 1, 1}, //
      {1, 1, 1, 1, 1, 1, 1}, //
      {1, 1, 1, 1, 1, 1, 1}, //
      {1, 1, 1, 1, 1, 1, 1}  //
  };

  mln::image2d<float> ref = {
      {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}, //
      {1.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 1.0f}, //
      {1.0f, 2.0f, 1.5f, 1.0f, 1.5f, 2.0f, 1.0f}, //
      {1.0f, 2.0f, 1.0f, 0.0f, 1.0f, 2.0f, 1.0f}, //
      {1.0f, 2.0f, 1.5f, 1.0f, 1.5f, 2.0f, 1.0f}, //
      {1.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 1.0f}, //
      {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}  //
  };


  mln::se::wmask2d weights = {{1.5f, 1.0f, 1.5f}, //
                              {1.0f, 0.0f, 1.0f}, //
                              {1.5f, 1.0f, 1.5f}};

  mln::image2d<float> res = mln::labeling::chamfer_distance_transform<float>(f, weights);

  ASSERT_IMAGES_EQ_EXP(res, ref);
}

TEST(Transform, chamfer_distance_transform_bg_is_one_weighted_distance_transform_float)
{

  mln::image2d<int> f = {
      {1, 1, 1, 1, 1, 1, 1}, //
      {1, 1, 1, 1, 1, 1, 1}, //
      {1, 1, 1, 1, 1, 1, 1}, //
      {1, 1, 1, 0, 1, 1, 1}, //
      {1, 1, 1, 1, 1, 1, 1}, //
      {1, 1, 1, 1, 1, 1, 1}, //
      {1, 1, 1, 1, 1, 1, 1}  //
  };

  mln::image2d<float> ref = {
      {4.5f, 4.0f, 3.5f, 3.0f, 3.5f, 4.0f, 4.5f}, //
      {4.0f, 3.0f, 2.5f, 2.0f, 2.5f, 3.0f, 4.0f}, //
      {3.5f, 2.5f, 1.5f, 1.0f, 1.5f, 2.5f, 3.5f}, //
      {3.0f, 2.0f, 1.0f, 0.0f, 1.0f, 2.0f, 3.0f}, //
      {3.5f, 2.5f, 1.5f, 1.0f, 1.5f, 2.5f, 3.5f}, //
      {4.0f, 3.0f, 2.5f, 2.0f, 2.5f, 3.0f, 4.0f}, //
      {4.5f, 4.0f, 3.5f, 3.0f, 3.5f, 4.0f, 4.5f}, //
  };


  mln::se::wmask2d weights = {{1.5f, 1.0f, 1.5f}, //
                              {1.0f, 0.0f, 1.0f},
                              {1.5f, 1.0f, 1.5f}};

  mln::image2d<float> res =
      mln::labeling::chamfer_distance_transform<float>(f, weights, true);

  ASSERT_IMAGES_EQ_EXP(res, ref);
}

TEST(Transform, chamfer_distance_transform_bg_is_one_weighted_distance_transform_int_approx_5x5)
{

  mln::image2d<int> f = {
      {1, 1, 1, 1, 1, 1, 1}, //
      {1, 1, 1, 1, 1, 1, 1}, //
      {1, 1, 1, 1, 1, 1, 1}, //
      {1, 1, 1, 0, 1, 1, 1}, //
      {1, 1, 1, 1, 1, 1, 1}, //
      {1, 1, 1, 1, 1, 1, 1}, //
      {1, 1, 1, 1, 1, 1, 1}  //
  };

  constexpr int a = 5, b = 7, c = 11;
  mln::image2d<int>  ref = {
      {21, 18, 16, 15, 16, 18, 21}, //
      {18, 14, 11, 10, 11, 14, 18}, //
      {16, 11, 7, 5, 7, 11, 16},    //
      {15, 10, 5, 0, 5, 10, 15},    //
      {16, 11, 7, 5, 7, 11, 16},    //
      {18, 14, 11, 10, 11, 14, 18}, //
      {21, 18, 16, 15, 16, 18, 21}  //
  };


  mln::se::wmask2d weights = {{0, c, 0, c, 0}, //
                              {c, b, a, b, c},
                              {0, a, 0, a, 0},
                              {c, b, a, b, c},
                              {0, c, 0, c, 0}};


  mln::image2d<int> res = mln::labeling::chamfer_distance_transform(f, weights, true);

  ASSERT_IMAGES_EQ_EXP(res, ref);
}
