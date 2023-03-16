#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/for_each.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/operators.hpp>

#include <utility>
#include <tbb/global_control.h>
#include <gtest/gtest.h>

TEST(Core, Algorithm_For_Each)
{
  using namespace mln::view::ops;

  mln::image2d<uint8_t> ima = {{1, 2, 3}, {4, 5, 6}};

  mln::image2d<uint8_t> ref = {{2, 3, 4}, {5, 6, 7}};

  mln::for_each(ima, [](uint8_t& x) { x += 1; });
  ASSERT_TRUE(mln::all_of(ima == ref));
}


TEST(Core, Algorithm_For_Each_Side_Effects)
{
  using namespace mln::view::ops;

  mln::image2d<uint8_t> ima = {{1, 2, 3}, {4, 5, 6}};


  int  sum = 0;
  auto g   = [&sum](uint8_t v) { sum += v; };
  mln::for_each(ima, g);
  ASSERT_EQ(6 * 7 / 2, sum);
}

TEST(Core, Algorithm_For_Each_Parallel)
{
  using namespace mln::view::ops;
  tbb::global_control init(tbb::global_control::max_allowed_parallelism, std::thread::hardware_concurrency());

  mln::image2d<uint8_t> ima = {{1, 2, 3}, {4, 5, 6}};
  mln::image2d<uint8_t> ref = {{2, 3, 4}, {5, 6, 7}};

  mln::parallel::for_each(ima, [](uint8_t& x) { x += 1; });
  ASSERT_TRUE(mln::all_of(ima == ref));
}
