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

TEST(Core, Parallel_transform)
{
  mln::experimental::image2d<uint8_t> ima = {{1, 2, 3}, {4, 5, 6}};
  mln::experimental::image2d<uint8_t> ref = {{2, 3, 4}, {5, 6, 7}};

  mln::TransformParallel transform(ima, ima, [](uint8_t x){ return x + 1; });
  mln::parallel_execute2d(transform);
  ASSERT_IMAGES_EQ_EXP(ima, ref);
}

TEST(Core, Parallel_copy)
{
  mln::experimental::image2d<uint8_t> ima = {{12, 2, 93}, {24, 75, 6}};
  mln::experimental::image2d<uint8_t> ref = {{2, 3, 4}, {5, 6, 7}};

  mln::CopyParallel copy(ref, ima);
  mln::parallel_execute2d(copy);
  ASSERT_IMAGES_EQ_EXP(ima, ref);
}

TEST(Core, Parallel_paste)
{
  mln::experimental::image2d<uint8_t> ima = {{12, 2, 93}, {24, 75, 6}};
  mln::experimental::image2d<uint8_t> ref = {{2, 3, 4}, {5, 6, 7}};

  mln::PasteParallel paste(ref, ima);
  mln::parallel_execute2d(paste);
  ASSERT_IMAGES_EQ_EXP(ima, ref);
}

TEST(Core, Parallel_fill)
{
  mln::experimental::image2d<uint8_t> ima = {{12, 2, 93}, {24, 75, 6}};
  mln::experimental::image2d<uint8_t> ref = {{1, 1, 1}, {1, 1, 1}};
  int value = 1;

  mln::FillParallel fill(ima, value);
  mln::parallel_execute2d(fill);
  ASSERT_IMAGES_EQ_EXP(ima, ref);
}