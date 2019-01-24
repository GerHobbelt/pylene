#include <mln/core/image/image2d.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/algorithm/sort.hpp>

#include <gtest/gtest.h>

TEST(Core, Algorithm_Sort_UInt8)
{
  using namespace mln;

  const image2d<std::uint8_t> ima = {{1, 4, 2}, {3, 6, 8}};

  std::vector<mln::point2d> points = mln::experimental::sort_points(ima);
  std::vector<mln::point2d> ref = {{0, 0}, {0, 2}, {1, 0}, {0, 1}, {1, 1}, {1, 2}};
  ASSERT_EQ(ref, points);
}




TEST(Core, Algorithm_Sort_Decreasing_UInt8)
{
  using namespace mln;

  const image2d<std::uint8_t> ima = {{1, 4, 2}, {3, 6, 8}};

  std::vector<mln::point2d> points = mln::experimental::sort_points(ima, std::greater<std::uint8_t>());
  std::vector<mln::point2d> ref = {{0, 0}, {0, 2}, {1, 0}, {0, 1}, {1, 1}, {1, 2}};
  std::reverse(ref.begin(), ref.end());
  ASSERT_EQ(ref, points);
}



TEST(Core, Algorithm_Sort_RGB8)
{
  using namespace mln;

  const image2d<mln::rgb8> ima = {
    { {10, 2, 6}, {4, 8, 5}, {10, 1, 8} },
    { {10, 2, 4}, {4, 6, 10}, {3, 4, 69} }
  };

  std::vector<mln::point2d> points = mln::experimental::sort_points(ima, mln::lexicographicalorder_less<mln::rgb8>());
  std::vector<mln::point2d> ref = {{1, 2}, {1, 1}, {0, 1}, {0, 2}, {1, 0}, {0, 0}};
  ASSERT_EQ(ref, points);
}



