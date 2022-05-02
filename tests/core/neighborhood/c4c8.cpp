#include <mln/core/image/ndimage.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighborhood/c4c8.hpp>

#include <gtest/gtest.h>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/take_last.hpp>
#include <range/v3/view/zip.hpp>

#include <iostream>
#include <vector>


static_assert(mln::concepts::Neighborhood<mln::c4_t, mln::point2d>);
static_assert(not mln::concepts::SeparableStructuringElement<mln::c4_t, mln::point2d>);
static_assert(not mln::concepts::DecomposableStructuringElement<mln::c4_t, mln::point2d>);
static_assert(not mln::concepts::IncrementalStructuringElement<mln::c4_t, mln::point2d>);

TEST(Core, c4_point)
{
  mln::point2d p{3, 7};
  auto         nbh = mln::c4c8_t(mln::c2d_type::C4);

  // Complete neighborhood
  for (auto&& [q, q_ref] : ::ranges::views::zip(nbh(p), mln::c4(p)))
    ASSERT_EQ(q, q_ref);

  // Before neighboors
  for (auto&& [q, q_ref] : ::ranges::views::zip(nbh.before(p), mln::c4.before(p)))
    ASSERT_EQ(q, q_ref);

  // After neighboors
  for (auto&& [q, q_ref] : ::ranges::views::zip(nbh.after(p), mln::c4.after(p)))
    ASSERT_EQ(q, q_ref);
}

TEST(Core, c8_point)
{
  // The order is different between mln::c8 and its dynamic version to use the stride view
  std::vector<mln::point2d> ref{{2, 6}, {3, 6}, {4, 6}, {2, 7}, {2, 8}, {4, 7}, {4, 8}, {3, 8}};
  mln::point2d              p{3, 7};
  auto                      nbh = mln::c4c8_t(mln::c2d_type::C8);

  // Complete neighborhood
  auto comp = ::ranges::to_vector(nbh(p));
  ASSERT_EQ(ref, comp);

  // Before neighboors
  comp = ::ranges::to_vector(nbh.before(p));
  ASSERT_EQ(::ranges::to_vector(ref | ::ranges::views::take(4)), comp);

  // After neighboors
  comp = ::ranges::to_vector(nbh.after(p));
  ASSERT_EQ(::ranges::to_vector(ref | ::ranges::views::take_last(4)), comp);
}