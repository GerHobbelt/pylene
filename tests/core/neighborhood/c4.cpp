#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c4.hpp>

#include <gtest/gtest.h>
#include <range/v3/range/conversion.hpp>
#include <vector>


static_assert(mln::concepts::Neighborhood<mln::c4_t, mln::point2d>);
static_assert(not mln::concepts::SeparableStructuringElement<mln::c4_t, mln::point2d>);
static_assert(not mln::concepts::DecomposableStructuringElement<mln::c4_t, mln::point2d>);
static_assert(not mln::concepts::IncrementalStructuringElement<mln::c4_t, mln::point2d>);


TEST(Core, c4_point)
{
  std::vector<mln::point2d> refs = {{-1, -2}, {-2, -1}, {0, -1}, {-1, 0}};

  mln::point2d p   = {-1, -1};
  auto         nbh = ::ranges::to_vector(mln::c4(p));

  ASSERT_EQ(refs, nbh);
}


TEST(Core, c4_pixel)
{
  mln::image2d<int> f = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};


  mln::point2d p   = {1, 1};
  auto                       pix = f.pixel_at(p);

  std::vector<mln::point2d> points = {{1, 0}, {0, 1}, {2, 1}, {1, 2}};
  std::vector<int>                        vals   = {2, 4, 6, 8};

  int i = 0;
  for (auto&& px : mln::c4(pix))
  {
    EXPECT_EQ(vals[i], px.val());
    EXPECT_EQ(points[i], px.point());
    i++;
  }
}
