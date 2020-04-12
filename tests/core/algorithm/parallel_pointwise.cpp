#include <mln/core/algorithm/parallel_pointwise.hpp>

#include <mln/core/image/experimental/ndimage.hpp>
#include <utility>

#include <fixtures/ImageCompare/image_compare.hpp>


#include <gtest/gtest.h>


TEST(Core, Parallel_foreach)
{
  mln::experimental::image2d<int> ima = {{1, 2, 3}, {4, 5, 6}};
  mln::experimental::image2d<int> ref = {{2, 3, 4}, {5, 6, 7}};

  mln::ForEachParallel foreach(ima, [](int& x){ x += 1; });
  mln::parallel_execute2d(foreach);
  ASSERT_IMAGES_EQ_EXP(ima, ref);
}
