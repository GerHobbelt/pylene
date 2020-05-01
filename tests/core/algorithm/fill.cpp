#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/image/view/operators.hpp>

#include <tbb/task_scheduler_init.h>
#include <gtest/gtest.h>

TEST(Core, Algorithm_Fill)
{
  using namespace mln::view::ops;

  mln::experimental::image2d<std::uint8_t> ima(10, 10);
  mln::fill(ima, 69);

  ASSERT_TRUE(mln::all_of(ima == 69));
}

TEST(Core, Algorithm_Fill_Parallel)
{
  using namespace mln::view::ops;

  tbb::task_scheduler_init init;
  mln::experimental::image2d<std::uint8_t> ima(10, 10);
  mln::parallel::fill(ima, 69);

  ASSERT_TRUE(mln::all_of(ima == 69));
}
