#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c6.hpp>
#include <mln/core/algorithm/iota.hpp>

#include <gtest/gtest.h>

#include <range/v3/range/conversion.hpp>
#include <vector>


static_assert(mln::concepts::Neighborhood<mln::c6_t, mln::point3d>);
static_assert(not mln::concepts::SeparableStructuringElement<mln::c6_t, mln::point3d>);
static_assert(not mln::concepts::DecomposableStructuringElement<mln::c6_t, mln::point3d>);
static_assert(not mln::concepts::IncrementalStructuringElement<mln::c6_t, mln::point3d>);


TEST(Core, c6_point)
{
  std::vector<mln::point3d> refs = {
      // clang-format off
    {-1, -1, -2},
    {-1, -2, -1},
    {-2, -1, -1},
    {+0, -1, -1},
    {-1, +0, -1},
    {-1, -1, +0}
      // clang-format on
  };

  mln::point3d p   = {-1, -1, -1};

  auto nbh = ::ranges::to_vector(mln::c6(p));

  ASSERT_EQ(refs, nbh);
}


TEST(Core, c6_pixel)
{
  mln::image3d<int> f(3, 3, 3);
  mln::iota(f, 0);


  mln::point3d p   = {1, 1, 1};
  auto         pix = f.new_pixel_at(p);

  std::vector<mln::point3d> points = {
    // clang-format off
    {+1, +1, +0},
    {+1, +0, +1},
    {+0, +1, +1},
    {+2, +1, +1},
    {+1, +2, +1},
    {+1, +1, +2}
    // clang-format on
  };
  

  std::vector<int>          vals   = {4, 10, 12, 14, 16, 22};

  int i = 0;
  for (auto&& px : mln::c6(pix))
  {
    EXPECT_EQ(vals[i], px.val());
    EXPECT_EQ(points[i], px.point());
    i++;
  }
  ASSERT_EQ(6, i);
}
