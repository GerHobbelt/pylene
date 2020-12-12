#include <mln/morpho/hats.hpp>

#include <mln/core/colors.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/se/rect2d.hpp>
#include <mln/io/imread.hpp>

#include <tbb/task_scheduler_init.h>

#include <fixtures/ImageCompare/image_compare.hpp>
#include <fixtures/ImagePath/image_path.hpp>

#include <gtest/gtest.h>

TEST(Morpho, bottom_hat)
{
  tbb::task_scheduler_init init;
  mln::image2d<uint8_t> ima;
  mln::io::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);
  auto win = mln::se::rect2d(3, 3);

  auto closing = mln::morpho::closing(ima, win);
  auto ref = mln::transform(ima, closing, mln::morpho::details::grad_op<uint8_t>());

  auto out = mln::morpho::bottom_hat(ima, win);

  ASSERT_IMAGES_EQ_EXP(out, ref);
}

TEST(Morpho, bottom_hat_parallel)
{
  tbb::task_scheduler_init init;
  mln::image2d<uint8_t> ima;
  mln::io::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);
  auto win = mln::se::rect2d(3, 3);

  auto ref = mln::morpho::bottom_hat(ima, win);
  auto out = mln::morpho::parallel::bottom_hat(ima, win);

  ASSERT_IMAGES_EQ_EXP(out, ref);
}

TEST(Morpho, top_hat)
{
  tbb::task_scheduler_init init;
  mln::image2d<uint8_t> ima;
  mln::io::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);
  auto win = mln::se::rect2d(3, 3);

  auto opening = mln::morpho::opening(ima, win);
  auto ref = mln::transform(opening, ima, mln::morpho::details::grad_op<uint8_t>());

  auto out = mln::morpho::top_hat(ima, win);

  ASSERT_IMAGES_EQ_EXP(out, ref);
}

TEST(Morpho, top_hat_parallel)
{
  tbb::task_scheduler_init init;
  mln::image2d<uint8_t> ima;
  mln::io::imread(fixtures::ImagePath::concat_with_filename("small.pgm"), ima);
  auto win = mln::se::rect2d(3, 3);

  auto ref = mln::morpho::top_hat(ima, win);
  auto out = mln::morpho::parallel::top_hat(ima, win);

  ASSERT_IMAGES_EQ_EXP(out, ref);
}