#include <mln/core/rangev3/view/zip.hpp>

#include <mln/core/box.hpp>
#include <mln/core/rangev3/mdindex.hpp>
#include <mln/core/rangev3/mdspan.hpp>
#include <mln/core/rangev3/rows.hpp>
#include <mln/core/rangev3/view/reverse.hpp>

#include <array>
#include <vector>

#include <gtest/gtest.h>


TEST(Core, zip_container)
{
  std::vector<int> ref = {0, 1, 2, 3, 4};
  std::vector<int> a   = {0, 1, 2, 3, 4};
  std::vector<int> b   = {0, 0, 0, 0, 0};

  auto rng = mln::ranges::view::zip(a, b);

  static_assert(::ranges::cpp20::forward_range<decltype(rng)>);

  for (auto [vin, vout] : rng)
    vout = vin;

  EXPECT_EQ(ref, a);
  EXPECT_EQ(ref, b);
}

TEST(Core, zip_mdrange_readonly)
{

  mln::experimental::box2d multi_ind0 = {{0, 0}, {3, 2}};
  mln::experimental::box2d multi_ind1 = {{2, 1}, {5, 3}};
  mln::experimental::box2d multi_ind2 = {{3, 2}, {6, 4}};

  std::vector<mln::experimental::point2d> ref = {
    {0, 0}, {1, 0}, {2, 0}, //
    {0, 1}, {1, 1}, {2, 1}, //
    {0, 2}, {1, 2}, {2, 2}  //
  };

  auto rng = mln::ranges::view::zip(multi_ind0, multi_ind1, multi_ind2);


  static_assert(mln::ranges::MDRange<decltype(rng)>);

  auto refv = std::begin(ref);
  for (auto&& r : rng.rows())
    for (auto [v1, v2, v3] : r)
    {
      ASSERT_EQ(*refv, v1);
      ASSERT_EQ((*refv + mln::experimental::point2d{2, 1}), v2);
      ASSERT_EQ((*refv + mln::experimental::point2d{3, 2}), v3);
      refv++;
    }
}

TEST(Core, zip_mdrange_write)
{
  std::vector<int> x_data = {1, 2, 3, 6, 5, 4};
  std::vector<int> y_data = {1, 2, 3, 6, 5, 4};

  mln::ranges::mdspan<int, 2> X(x_data.data(), {3, 2}, {1, 3});
  mln::ranges::mdspan<int, 2> Y(y_data.data(), {3, 2}, {1, 3});

  auto ind0 = mln::experimental::box2d(4, 3);
  auto z    = mln::ranges::view::zip(X, Y, ind0);

  static_assert(mln::ranges::MDRange<decltype(z)>);

  for (auto&& r : z.rows())
  {
    for (auto [x, y, p] : r)
    {
      x    = p[0]; // By ref
      y    = p[1]; // By ref
      p[0] = 42;   // No effect
    }
  }

  std::vector<int> refY = {0, 0, 0, 1, 1, 1};
  std::vector<int> refX = {0, 1, 2, 0, 1, 2};
  EXPECT_EQ(refX, x_data);
  EXPECT_EQ(refY, y_data);
}


TEST(Range, zipped_reversed_range)
{
  mln::experimental::box2d multi_ind0 = {{0, 0}, {3, 2}};
  mln::experimental::box2d multi_ind1 = {{2, 1}, {5, 3}};

  std::vector<mln::experimental::point2d> ref = {
    {0, 0}, {1, 0}, {2, 0}, //
    {0, 1}, {1, 1}, {2, 1}, //
  };


  auto rng = mln::ranges::view::zip(multi_ind0, multi_ind1);
  static_assert(mln::ranges::MDBidirectionalRange<decltype(rng)>);

  auto rit = ::ranges::rbegin(ref);

  auto rrng = mln::ranges::view::reverse(rng);
  for (auto&& rows : rrng.rows())
    for (auto [p1, p2] : rows)
    {
      ASSERT_EQ(p1, *rit++);
    }

  ASSERT_EQ(rit, ::ranges::rend(ref));
}


/* FIXME
TEST(Core, zip_segmented_and_nonsegmented_should_fail)
{
  mln::experimental::box2d multi_ind0 = {{0, 0}, {3, 2}};

  std::vector<mln::experimental::point2d> ref = {
      {0, 0}, {0, 1}, {0, 2}, //
      {1, 0}, {1, 1}, {1, 2}, //
  };

  auto z = mln::ranges::view::zip(ref, multi_ind0);
  for (auto [v1, v2] : z)
    ASSERT_EQ(v1, v2);
}
*/
