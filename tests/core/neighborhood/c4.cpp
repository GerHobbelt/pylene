#include <mln/core/image/image2d.hpp>
#include <mln/core/neighborhood/c4.hpp>

#include <gtest/gtest.h>
#include <range/v3/to_container.hpp>
#include <vector>


#ifdef PYLENE_CONCEPT_TS_ENABLED
static_assert(mln::concepts::Neighborhood<mln::experimental::c4_t, mln::point2d>);
static_assert(not mln::concepts::SeparableStructuringElement<mln::experimental::c4_t, mln::point2d>);
static_assert(not mln::concepts::DecomposableStructuringElement<mln::experimental::c4_t, mln::point2d>);
static_assert(not mln::concepts::IncrementalStructuringElement<mln::experimental::c4_t, mln::point2d>);
#endif


TEST(Core, c4_point)
{
  std::vector<mln::point2d> refs = {{-2, -1}, {-1, -2}, {-1, 0}, {0, -1}};

  mln::point2d p   = {-1, -1};
  auto         nbh = ::ranges::to_vector(mln::experimental::c4(p));

  ASSERT_EQ(refs, nbh);
}


TEST(Core, c4_pixel)
{
  mln::image2d<int> f = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};

  // FIXME
  mln::point2d p   = {1, 1};
  auto         pix = f.new_pixel_at(p);

  std::vector<mln::point2d> points = {{0, 1}, {1, 0}, {1, 2}, {2, 1}};
  std::vector<int>          vals   = {2, 4, 6, 8};

  int i = 0;
  for (auto&& px : mln::experimental::c4(pix))
  {
    EXPECT_EQ(vals[i], px.val());
    EXPECT_EQ(points[i], px.point());
    i++;
  }
}
