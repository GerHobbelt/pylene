#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/neighborhood/c6.hpp>
#include <mln/core/algorithm/iota.hpp>

#include <gtest/gtest.h>

#include <range/v3/range/conversion.hpp>
#include <vector>


#ifdef PYLENE_CONCEPT_TS_ENABLED
static_assert(mln::concepts::Neighborhood<mln::experimental::c6_t, mln::experimental::point3d>);
static_assert(not mln::concepts::SeparableStructuringElement<mln::experimental::c6_t, mln::experimental::point3d>);
static_assert(not mln::concepts::DecomposableStructuringElement<mln::experimental::c6_t, mln::experimental::point3d>);
static_assert(not mln::concepts::IncrementalStructuringElement<mln::experimental::c6_t, mln::experimental::point3d>);
#endif


TEST(Core, c6_point)
{
  std::vector<mln::experimental::point3d> refs = {
      // clang-format off
    {-1, -1, -2},
    {-1, -2, -1},
    {-2, -1, -1},
    {+0, -1, -1},
    {-1, +0, -1},
    {-1, -1, +0}
      // clang-format on
  };

  mln::experimental::point3d p   = {-1, -1, -1};

  auto nbh = ::ranges::to_vector(mln::experimental::c6(p));

  ASSERT_EQ(refs, nbh);
}


TEST(Core, c6_pixel)
{
  mln::experimental::image3d<int> f(3, 3, 3);
  mln::iota(f, 0);


  mln::experimental::point3d p   = {1, 1, 1};
  auto         pix = f.new_pixel_at(p);

  std::vector<mln::experimental::point3d> points = {
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
  for (auto&& px : mln::experimental::c6(pix))
  {
    EXPECT_EQ(vals[i], px.val());
    EXPECT_EQ(points[i], px.point());
    i++;
  }
  ASSERT_EQ(6, i);
}
