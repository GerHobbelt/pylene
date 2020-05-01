#include <mln/core/algorithm/copy.hpp>

#include <mln/core/algorithm/equal.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/image/experimental/ndimage.hpp>

#include <tbb/task_scheduler_init.h>
#include <gtest/gtest.h>


TEST(Core, Algorithm_Copy)
{
  mln::experimental::image2d<std::uint8_t> ima(10, 10);
  mln::experimental::image2d<std::uint8_t> out(10, 10);
  mln::iota(ima, 0);
  mln::experimental::copy(ima, out);

  ASSERT_TRUE(mln::equal(ima, out));
}

TEST(Core, Algorithm_Copy_Parallel)
{
  mln::experimental::image2d<uint8_t> ima = {{12, 2, 93}, {24, 75, 6}};
  mln::experimental::image2d<uint8_t> ref = {{2, 3, 4}, {5, 6, 7}};

  tbb::task_scheduler_init init;
  mln::experimental::parallel::copy(ref, ima);
  ASSERT_TRUE(mln::equal(ima, ref));
}
