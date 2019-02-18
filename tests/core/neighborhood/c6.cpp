#include <mln/core/image/image3d.hpp>
#include <mln/core/neighborhood/c6.hpp>
#include <mln/core/algorithm/iota.hpp>

#include <gtest/gtest.h>

#include <range/v3/to_container.hpp>
#include <vector>


#ifdef PYLENE_CONCEPT_TS_ENABLED
static_assert(mln::concepts::Neighborhood<mln::experimental::c6_t, mln::point3d>);
static_assert(not mln::concepts::SeparableStructuringElement<mln::experimental::c6_t, mln::point3d>);
static_assert(not mln::concepts::DecomposableStructuringElement<mln::experimental::c6_t, mln::point3d>);
static_assert(not mln::concepts::IncrementalStructuringElement<mln::experimental::c6_t, mln::point3d>);
#endif


TEST(Core, c6_point)
{
  std::vector<mln::point3d> refs = {{-2, -1, -1}, {-1, -2, -1}, {-1, -1, -2}, {-1, -1, +0}, {-1, +0, -1}, {+0, -1, -1}};

  mln::point3d p   = {-1, -1, -1};
  auto         nbh = ::ranges::to_vector(mln::experimental::c6(p));

  ASSERT_EQ(refs, nbh);
}


TEST(Core, c6_pixel)
{
  mln::image3d<int> f(3,3,3);
  mln::experimental::iota(f, 0);


  mln::point3d p   = {1, 1, 1};
  auto         pix = f.new_pixel_at(p);

  std::vector<mln::point3d> points = {{+0, +1, +1}, {+1, +0, +1}, {+1, +1, +0},
                                      {+1, +1, +2}, {+1, +2, +1}, {+2, +1, +1}};


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
