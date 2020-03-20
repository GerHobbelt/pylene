#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/algorithm/parallel_pointwise_canvas.hpp>
#include <mln/core/image/view/operators.hpp>

#include <utility>

#include <gtest/gtest.h>

TEST(Core, Parallel_foreach)
{
  using namespace mln::view::ops;
  mln::experimental::image2d<int> ima = {{1, 2, 3}, {4, 5, 6}};
  mln::experimental::image2d<int> ref = {{2, 3, 4}, {5, 6, 7}};

  std::cerr << "Before foreach: " << ima({0,0}) << "\n";
  mln::ApplyPointwise(ima, [](int& x){ x += 1; });
  std::cerr << "After foreach: " << ima({0,0}) << "\n";
  ASSERT_TRUE(mln::all_of(ima == ref));
}
