#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c8.hpp>

#include <gtest/gtest.h>
#include <range/v3/range/conversion.hpp>
#include <vector>


static_assert(mln::concepts::Neighborhood<mln::c8_t, mln::point2d>);
static_assert(not mln::concepts::SeparableStructuringElement<mln::c8_t, mln::point2d>);
static_assert(not mln::concepts::DecomposableStructuringElement<mln::c8_t, mln::point2d>);
static_assert(not mln::concepts::IncrementalStructuringElement<mln::c8_t, mln::point2d>);


TEST(Core, c4_point)
{
  // clang-format off
  std::vector<mln::point2d> refs = {
    {-2, -2}, {-1, -2}, {+0, -2},
    {-2, -1},           {+0, -1},
    {-2, +0}, {-1, +0}, {+0, +0},
  };
  // clang-format on

  auto p   = mln::point2d{-1, -1};
  auto nbh = ::ranges::to_vector(mln::c8(p));

  ASSERT_EQ(refs, nbh);
}


TEST(Core, c4_pixel)
{
  mln::image2d<int> f = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};

  auto p   = mln::point2d{1, 1};
  auto pix = f.pixel_at(p);

  // clang-format off
  std::vector<mln::point2d> points = {
    {+0, +0}, {+1, +0}, {+2, +0},
    {+0, +1},           {+2, +1},
    {+0, +2}, {+1, +2}, {+2, +2},
  };
  // clang-format on

  std::vector<int> vals = {1, 2, 3, 4, 6, 7, 8, 9};

  int i = 0;
  for (auto&& px : mln::c8(pix))
  {
    EXPECT_EQ(vals[i], px.val());
    EXPECT_EQ(points[i], px.point());
    i++;
  }
}
