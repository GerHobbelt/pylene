#include <mln/core/image/image3d.hpp>
#include <mln/core/neighborhood/c26.hpp>
#include <mln/core/algorithm/iota.hpp>

#include <gtest/gtest.h>

#include <range/v3/to_container.hpp>
#include <vector>


#ifdef PYLENE_CONCEPT_TS_ENABLED
static_assert(mln::concepts::Neighborhood<mln::experimental::c26_t, mln::point3d>);
static_assert(not mln::concepts::SeparableStructuringElement<mln::experimental::c26_t, mln::point3d>);
static_assert(not mln::concepts::DecomposableStructuringElement<mln::experimental::c26_t, mln::point3d>);
static_assert(not mln::concepts::IncrementalStructuringElement<mln::experimental::c26_t, mln::point3d>);
#endif


TEST(Core, c6_point)
{
  std::vector<mln::point3d> refs = {
    // clang-format off
    {-2,-2,-2}, {-2,-2,-1}, {-2,-2,+0},
    {-2,-1,-2}, {-2,-1,-1}, {-2,-1,+0},
    {-2,+0,-2}, {-2,+0,-1}, {-2,+0,+0},
    // ---
    {-1,-2,-2}, {-1,-2,-1}, {-1,-2,+0},
    {-1,-1,-2},             {-1,-1,+0},
    {-1,+0,-2}, {-1,+0,-1}, {-1,+0,+0},
    // ---
    {+0,-2,-2}, {+0,-2,-1}, {+0,-2,+0},
    {+0,-1,-2}, {+0,-1,-1}, {+0,-1,+0},
    {+0,+0,-2}, {+0,+0,-1}, {+0,+0,+0},
    // clang-format on
  };

  mln::point3d p   = {-1, -1, -1};
  auto         nbh = ::ranges::to_vector(mln::experimental::c26(p));

  ASSERT_EQ(refs, nbh);
}


TEST(Core, c26_pixel)
{
  mln::image3d<int> f(3,3,3);
  mln::iota(f, 0);


  mln::point3d p   = {1, 1, 1};
  auto         pix = f.new_pixel_at(p);

  std::vector<mln::point3d> points = {
    // clang-format off
    {+0,+0,+0}, {+0,+0,+1}, {+0,+0,+2},
    {+0,+1,+0}, {+0,+1,+1}, {+0,+1,+2},
    {+0,+2,+0}, {+0,+2,+1}, {+0,+2,+2},
    // ---
    {+1,+0,+0}, {+1,+0,+1}, {+1,+0,+2},
    {+1,+1,+0},             {+1,+1,+2},
    {+1,+2,+0}, {+1,+2,+1}, {+1,+2,+2},
    // ---
    {+2,+0,+0}, {+2,+0,+1}, {+2,+0,+2},
    {+2,+1,+0}, {+2,+1,+1}, {+2,+1,+2},
    {+2,+2,+0}, {+2,+2,+1}, {+2,+2,+2},
    // clang-format on
  };

  std::vector<int> vals = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12,
                           14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26};
  int i = 0;
  for (auto&& px : mln::experimental::c26(pix))
  {
    EXPECT_EQ(vals[i], px.val());
    EXPECT_EQ(points[i], px.point());
    i++;
  }
  ASSERT_EQ(26, i);
}
