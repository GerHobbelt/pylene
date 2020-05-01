#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/transform.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/image/view/operators.hpp>
#include <fixtures/ImageCompare/image_compare.hpp>

#include <utility>
#include <tbb/task_scheduler_init.h>
#include <gtest/gtest.h>

TEST(Core, Algorithm_Transform)
{
  using namespace mln::view::ops;

  mln::experimental::image2d<uint8_t> ima = {{1, 2, 3}, {4, 5, 6}};

  mln::experimental::image2d<uint8_t> ref = {{2, 3, 4}, {5, 6, 7}};

  auto out = mln::transform(ima, [](uint8_t x) -> uint8_t { return x + 1; });
  ASSERT_TRUE(mln::all_of(out == ref));
}


TEST(Core, Algorithm_Transform_LValue)
{
  using namespace mln::view::ops;
  using V = std::pair<uint8_t, uint8_t>;


  mln::experimental::image2d<V>       ima = {{{1, 0}, {2, 0}, {3, 0}}, {{4, 0}, {5, 0}, {6, 0}}};
  mln::experimental::image2d<uint8_t> ref = {{1, 2, 3}, {4, 5, 6}};


  mln::experimental::image2d<uint8_t> out = mln::transform(ima, &V::first);
  ASSERT_TRUE(mln::all_of(out == ref));
}

TEST(Core, Algorithm_Transform_Parallel)
{
  tbb::task_scheduler_init init;
  mln::experimental::image2d<uint8_t> ima = {{1, 2, 3}, {4, 5, 6}};
  mln::experimental::image2d<uint8_t> ref = {{2, 3, 4}, {5, 6, 7}};

  mln::parallel::transform(ima, ima, [](uint8_t x){ return x + 1; });
  ASSERT_IMAGES_EQ_EXP(ima, ref);
}
