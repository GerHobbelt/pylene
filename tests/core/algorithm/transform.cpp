#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/transform.hpp>
#include <mln/core/image/ndimage.hpp>
#include <fixtures/ImageCompare/image_compare.hpp>

#include <functional>
#include <utility>
#include <tbb/task_scheduler_init.h>
#include <gtest/gtest.h>

TEST(Core, Algorithm_Transform)
{
  mln::image2d<uint8_t> ima = {{1, 2, 3}, {4, 5, 6}};

  mln::image2d<uint8_t> ref = {{2, 3, 4}, {5, 6, 7}};

  auto out = mln::transform(ima, [](uint8_t x) -> uint8_t { return x + 1; });
  ASSERT_IMAGES_EQ_EXP(out, ref);
}

TEST(Core, Algorithm_Transform_3args_UnaryFunction)
{
  mln::image2d<uint8_t> ima = {{1, 2, 3}, {4, 5, 6}};
  mln::image2d<uint8_t> out = {{0, 0, 0}, {0, 0, 0}};

  mln::image2d<uint8_t> ref = {{2, 3, 4}, {5, 6, 7}};

  mln::transform(ima, out, [](uint8_t x) -> uint8_t { return x + 1; });
  ASSERT_IMAGES_EQ_EXP(out, ref);
}

TEST(Core, Algorithm_Transform_3args_BinaryFunction)
{
  mln::image2d<uint8_t> ima1 = {{2, 3, 4}, {5, 6, 8}};
  mln::image2d<uint8_t> ima2 = {{1, 2, 3}, {4, 5, 6}};

  mln::image2d<uint8_t> ref = {{1, 1, 1}, {1, 1, 2}};

  mln::image2d<uint8_t> out = mln::transform(ima1, ima2, std::minus<uint8_t>() );
  ASSERT_IMAGES_EQ_EXP(out, ref);
}


TEST(Core, Algorithm_Transform_LValue)
{
  using V = std::pair<uint8_t, uint8_t>;


  mln::image2d<V>       ima = {{{1, 0}, {2, 0}, {3, 0}}, {{4, 0}, {5, 0}, {6, 0}}};
  mln::image2d<uint8_t> ref = {{1, 2, 3}, {4, 5, 6}};


  mln::image2d<uint8_t> out = mln::transform(ima, &V::first);
  ASSERT_IMAGES_EQ_EXP(out, ref);
}

TEST(Core, Algorithm_Transform_Parallel_Unary_with_out)
{
  tbb::task_scheduler_init init;
  mln::image2d<uint8_t> ima = {{1, 2, 3}, {4, 5, 6}};
  mln::image2d<uint8_t> ref = {{2, 3, 4}, {5, 6, 7}};

  mln::parallel::transform(ima, ima, [](uint8_t x){ return x + 1; });
  ASSERT_IMAGES_EQ_EXP(ima, ref);
}

TEST(Core, Algorithm_Transform_Parallel_Unary_no_out)
{
  tbb::task_scheduler_init init;
  mln::image2d<uint8_t> ima = {{1, 2, 3}, {4, 5, 6}};
  mln::image2d<uint8_t> ref = {{2, 3, 4}, {5, 6, 7}};

  auto out = mln::parallel::transform(ima, [](uint8_t x) -> uint8_t { return x + 1; });
  ASSERT_IMAGES_EQ_EXP(out, ref);
}

TEST(Core, Algorithm_Transform_Parallel_Binary_with_out)
{
  tbb::task_scheduler_init init;
  mln::image2d<uint8_t> ima1 = {{2, 3, 4}, {5, 6, 8}};
  mln::image2d<uint8_t> ima2 = {{1, 2, 3}, {4, 5, 6}};
  mln::image2d<uint8_t> out = {{0, 0, 0}, {0, 0, 0}};
  mln::image2d<uint8_t> ref = {{1, 1, 1}, {1, 1, 2}};

  mln::parallel::transform(ima1, ima2, out, std::minus<uint8_t>() );
  ASSERT_IMAGES_EQ_EXP(out, ref);
}

TEST(Core, Algorithm_Transform_Parallel_Binary_no_out)
{
  tbb::task_scheduler_init init;
  mln::image2d<uint8_t> ima1 = {{2, 3, 4}, {5, 6, 8}};
  mln::image2d<uint8_t> ima2 = {{1, 2, 3}, {4, 5, 6}};
  mln::image2d<uint8_t> ref = {{1, 1, 1}, {1, 1, 2}};
  
  mln::image2d<uint8_t> out = mln::parallel::transform(ima1, ima2, std::minus<uint8_t>() );

  ASSERT_IMAGES_EQ_EXP(out, ref);
}
