#include <mln/core/image/image2d.hpp>
#include <mln/core/neighborhood/c8.hpp>

#include <gtest/gtest.h>
#include <range/v3/to_container.hpp>
#include <vector>

TEST(Core, c4_point)
{
  // clang-format off
  std::vector<mln::point2d> refs = {
      {-2, -2}, {-2, -1}, {-2, +0},
      {-1, -2},           {-1, +0},
      {+0, -2}, {+0, -1}, {+0, +0},
  };
  // clang-format on

  mln::point2d p   = {-1, -1};
  auto         nbh = ::ranges::to_vector(mln::c8_new(p));

  ASSERT_EQ(refs, nbh);
}


TEST(Core, c4_pixel)
{
  mln::image2d<int> f = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};

  // FIXME
  mln::point2d p   = {1, 1};
  auto         pix = f.new_pixel_at(p);

  // clang-format off
  std::vector<mln::point2d> points = {
      {+0, +0}, {+0, +1}, {+0, +2},
      {+1, +0},           {+1, +2},
      {+2, +0}, {+2, +1}, {+2, +2},
  };
  // clang-format on

  std::vector<int> vals = {1, 2, 3, 4, 6, 7, 8, 9};

  int i = 0;
  for (auto&& px : mln::c8_new(pix))
  {
    EXPECT_EQ(vals[i], px.val());
    EXPECT_EQ(points[i], px.point());
    i++;
  }
}
