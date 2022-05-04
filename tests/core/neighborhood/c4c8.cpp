#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighborhood/c4c8.hpp>
#include <mln/core/neighborhood/c8.hpp>

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/take_last.hpp>
#include <range/v3/view/zip.hpp>

#include <gtest/gtest.h>

#include <iostream>
#include <vector>


static_assert(mln::concepts::Neighborhood<mln::c4c8_t, mln::point2d>);
static_assert(not mln::concepts::SeparableStructuringElement<mln::c4c8_t, mln::point2d>);
static_assert(not mln::concepts::DecomposableStructuringElement<mln::c4c8_t, mln::point2d>);
static_assert(not mln::concepts::IncrementalStructuringElement<mln::c4c8_t, mln::point2d>);

TEST(Core, c4c8_c4_point)
{
  mln::point2d              p{3, 7};
  auto                      nbh = mln::c4c8_t(mln::c4);
  std::vector<mln::point2d> ref{{2, 7}, {3, 6}, {4, 7}, {3, 8}};

  // Complete neighborhood
  auto comp = ::ranges::to_vector(nbh(p));
  ASSERT_EQ(comp, ref);

  // Before neighboors
  for (auto&& [q, q_ref] : ::ranges::views::zip(nbh.before(p), ref | ::ranges::views::take(2)))
    ASSERT_EQ(q, q_ref);

  // After neighboors
  for (auto&& [q, q_ref] : ::ranges::views::zip(nbh.after(p), ref | ::ranges::views::take_last(2)))
    ASSERT_EQ(q, q_ref);
}

TEST(Core, c4c8_c8_point)
{
  // The order is different between mln::c8 and its dynamic version to use the stride view
  std::vector<mln::point2d> ref{{2, 7}, {3, 6}, {4, 7}, {3, 8}, {2, 6}, {4, 6}, {4, 8}, {2, 8}};
  std::vector<mln::point2d> after_ref{{4, 7}, {3, 8}, {4, 8}, {2, 8}};
  std::vector<mln::point2d> before_ref{{2, 7}, {3, 6}, {2, 6}, {4, 6}};
  mln::point2d              p{3, 7};
  auto                      nbh = mln::c4c8_t(mln::c8);

  // Complete neighborhood
  auto comp = ::ranges::to_vector(nbh(p));

  ASSERT_EQ(ref, comp);

  // Before neighboors
  comp = ::ranges::to_vector(nbh.before(p));
  ASSERT_EQ(before_ref, comp);

  // After neighboors
  comp = ::ranges::to_vector(nbh.after(p));
  ASSERT_EQ(after_ref, comp);
}

TEST(Core, c4c8_c4_pixel)
{
  mln::image2d<std::uint8_t> ima{
      {1, 2, 3}, //
      {4, 5, 6}, //
      {7, 8, 9}  //
  };
  auto pix = ima.pixel_at(mln::point2d{1, 1});

  mln::c4c8_t nbh(mln::c4);

  std::vector<std::uint8_t> v_ref{4, 2, 6, 8};
  std::vector<mln::point2d> p_ref{{0, 1}, {1, 0}, {2, 1}, {1, 2}};

  int i = 0;
  for (auto&& qix : nbh(pix))
  {
    ASSERT_EQ(qix.val(), v_ref[i]);
    ASSERT_EQ(qix.point(), p_ref[i++]);
  }
}

TEST(Core, c4c8_c8_pixel)
{
  mln::image2d<std::uint8_t> ima{
      {1, 2, 3}, //
      {4, 5, 6}, //
      {7, 8, 9}  //
  };
  auto pix = ima.pixel_at(mln::point2d{1, 1});

  mln::c4c8_t nbh(mln::c8);

  std::vector<std::uint8_t> v_ref{4, 2, 6, 8, 1, 3, 9, 7};
  std::vector<mln::point2d> p_ref{{0, 1}, {1, 0}, {2, 1}, {1, 2}, {0, 0}, {2, 0}, {2, 2}, {0, 2}};

  int i = 0;
  for (auto&& qix : nbh(pix))
  {
    ASSERT_EQ(qix.val(), v_ref[i]);
    ASSERT_EQ(qix.point(), p_ref[i++]);
  }
}