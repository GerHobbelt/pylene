#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/clone.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/operators.hpp>

#include <tbb/global_control.h>
#include <gtest/gtest.h>

TEST(Core, Algorithm_Clone)
{
  using namespace mln::view::ops;

  const mln::image2d<uint8_t> ima = {{1, 2, 3}, {4, 5, 6}};
  auto                        out = mln::clone(ima);

  // Writing does not affect ima
  out({0, 0}) = 69;


  const mln::image2d<uint8_t> ref = {{69, 2, 3}, {4, 5, 6}};
  ASSERT_TRUE(mln::all_of(out == ref));
}

TEST(Core, Algorithm_Clone_Parallel)
{
  using namespace mln::view::ops;

  tbb::global_control init(tbb::global_control::max_allowed_parallelism, std::thread::hardware_concurrency());

  const mln::image2d<uint8_t> ima = {{1, 2, 3}, {4, 5, 6}};
  auto                        out = mln::parallel::clone(ima);

  // Writing does not affect ima
  out({0, 0}) = 69;

  const mln::image2d<uint8_t> ref = {{69, 2, 3}, {4, 5, 6}};
  ASSERT_TRUE(mln::all_of(out == ref));
}
