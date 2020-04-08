#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/neighborhood/c8.hpp>

#include <gtest/gtest.h>
#include <range/v3/range/conversion.hpp>
#include <vector>


static_assert(mln::concepts::Neighborhood<mln::experimental::c8_t, mln::experimental::point2d>);
static_assert(not mln::concepts::SeparableStructuringElement<mln::experimental::c8_t, mln::experimental::point2d>);
static_assert(not mln::concepts::DecomposableStructuringElement<mln::experimental::c8_t, mln::experimental::point2d>);
static_assert(not mln::concepts::IncrementalStructuringElement<mln::experimental::c8_t, mln::experimental::point2d>);


TEST(Core, c4_point)
{
  // clang-format off
  std::vector<mln::experimental::point2d> refs = {
    {-2, -2}, {-1, -2}, {+0, -2},
    {-2, -1},           {+0, -1},
    {-2, +0}, {-1, +0}, {+0, +0},
  };
  // clang-format on

  auto p   = mln::experimental::point2d{-1, -1};
  auto nbh = ::ranges::to_vector(mln::experimental::c8(p));

  ASSERT_EQ(refs, nbh);
}


TEST(Core, c4_pixel)
{
  mln::experimental::image2d<int> f = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};

  auto p   = mln::experimental::point2d{1, 1};
  auto pix = f.new_pixel_at(p);

  // clang-format off
  std::vector<mln::experimental::point2d> points = {
    {+0, +0}, {+1, +0}, {+2, +0},
    {+0, +1},           {+2, +1},
    {+0, +2}, {+1, +2}, {+2, +2},
  };
  // clang-format on

  std::vector<int> vals = {1, 2, 3, 4, 6, 7, 8, 9};

  int i = 0;
  for (auto&& px : mln::experimental::c8(pix))
  {
    EXPECT_EQ(vals[i], px.val());
    EXPECT_EQ(points[i], px.point());
    i++;
  }
}
