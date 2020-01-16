#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/algorithm/sort.hpp>

#include <gtest/gtest.h>


using P = mln::experimental::ndpoint<2, short>;

TEST(Core, Algorithm_Sort_UInt8)
{
  const mln::experimental::image2d<std::uint8_t> ima = {{1, 4, 2}, {3, 6, 8}};

  std::vector<P> points = mln::experimental::sort_points(ima);
  std::vector<P> ref    = {{0, 0}, {2, 0}, {0, 1}, {1, 0}, {1, 1}, {2, 1}};
  ASSERT_EQ(ref, points);
}




TEST(Core, Algorithm_Sort_Decreasing_UInt8)
{
  const mln::experimental::image2d<std::uint8_t> ima = {{1, 4, 2}, {3, 6, 8}};

  std::vector<P> points = mln::experimental::sort_points(ima, std::greater<std::uint8_t>());
  std::vector<P> ref = {{0, 0}, {2, 0}, {0, 1}, {1, 0}, {1, 1}, {2, 1}};

  std::reverse(ref.begin(), ref.end());
  ASSERT_EQ(ref, points);
}



TEST(Core, Algorithm_Sort_RGB8)
{
  const mln::experimental::image2d<mln::rgb8> ima = {
    { {10, 2, 6}, {4, 8, 5}, {10, 1, 8} },
    { {10, 2, 4}, {4, 6, 10}, {3, 4, 69} }
  };

  std::vector<P> points = mln::experimental::sort_points(ima, mln::lexicographicalorder_less<mln::rgb8>());
  std::vector<P> ref = {{2, 1}, {1, 1}, {1, 0}, {2, 0}, {0, 1}, {0, 0}};

  ASSERT_EQ(ref, points);
}



