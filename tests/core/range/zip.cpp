#include <mln/core/domain/box.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/range/zip.hpp>
#include <mln/core/rangev3/multi_indices.hpp>
#include <mln/core/rangev3/multi_span.hpp>
#include <mln/core/rangev3/rows.hpp>
#include <mln/core/rangev3/view/zip.hpp>
#include <range/v3/to_container.hpp>

#include <array>
#include <iostream>

#include <gtest/gtest.h>


namespace concepts = mln::concepts;

TEST(Core, ziprange)
{

  using namespace mln;

  std::array<int, 4> x{{2, 5, 15, 22}};
  std::array<int, 4> y{{-2, -5, -15, -22}};

  ASSERT_EQ(rng::size(x), 4u);

  auto z = rng::zip(x, y);

  {
    int i = 0;
    mln_foreach (auto v, z)
    {
      ASSERT_EQ(std::get<0>(v), x[i]);
      ASSERT_EQ(std::get<1>(v), y[i++]);
    }
  }

  {
    int i = 0;
    mln_iter(pv, z);
    mln_forall (pv)
    {
      ASSERT_EQ(std::get<0>(*pv), x[i]);
      ASSERT_EQ(std::get<1>(*pv), y[i++]);
    }
  }
}

TEST(Core, zip_container)
{
  std::vector<int> ref = {0, 1, 2, 3, 4};
  std::vector<int> a   = {0, 1, 2, 3, 4};
  std::vector<int> b   = {0, 0, 0, 0, 0};

  auto rng = mln::ranges::view::zip(a, b);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(concepts::stl::ForwardRange<decltype(rng)>);
  static_assert(concepts::stl::OutputRange<decltype(rng), std::tuple<int, int>>);
#else
  static_assert(::ranges::ForwardRange<decltype(rng)>());
  static_assert(::ranges::OutputRange<decltype(rng), std::tuple<int, int>>());
#endif // PYLENE_CONCEPT_TS_ENABLED

  for (auto [vin, vout] : rng)
    vout = vin;

  EXPECT_EQ(ref, a);
  EXPECT_EQ(ref, b);
}

TEST(Core, zip_view_readonly)
{

  mln::box2d multi_ind0 = {{0, 0}, {2, 3}};
  mln::box2d multi_ind1 = {{1, 2}, {3, 5}};
  mln::box2d multi_ind2 = {{2, 3}, {4, 6}};

  std::vector<mln::point2d> ref = {
      {0, 0}, {0, 1}, {0, 2}, //
      {1, 0}, {1, 1}, {1, 2}, //
      {2, 0}, {2, 1}, {2, 2}  //
  };

  auto rng = mln::ranges::view::zip(multi_ind0, multi_ind1, multi_ind2);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(concepts::SegmentedRange<decltype(multi_ind0)>);
  static_assert(concepts::ReversibleRange<decltype(multi_ind0)>);
  static_assert(concepts::SegmentedRange<decltype(multi_ind1)>);
  static_assert(concepts::ReversibleRange<decltype(multi_ind1)>);
  static_assert(concepts::SegmentedRange<decltype(multi_ind2)>);
  static_assert(concepts::ReversibleRange<decltype(multi_ind2)>);
  static_assert(concepts::SegmentedRange<decltype(rng)>);
  static_assert(concepts::ReversibleRange<decltype(rng)>);
#else
  static_assert(::ranges::ForwardRange<decltype(multi_ind0)>());
  static_assert(::ranges::ForwardRange<decltype(multi_ind1)>());
  static_assert(::ranges::ForwardRange<decltype(multi_ind2)>());
  static_assert(::ranges::ForwardRange<decltype(rng)>());
#endif // PYLENE_CONCEPT_TS_ENABLED

  auto refv = std::begin(ref);
  for (auto [v1, v2, v3] : rng)
  {
    ASSERT_EQ(*refv, v1);
    ASSERT_EQ((*refv + mln::point2d{1, 2}), v2);
    ASSERT_EQ((*refv + mln::point2d{2, 3}), v3);
    refv++;
  }
}

TEST(Core, zip_view_readonly_rowwise)
{

  mln::box2d                multi_ind0 = {{0, 0}, {2, 3}};
  mln::box2d                multi_ind1 = {{1, 2}, {3, 5}};
  mln::box2d                multi_ind2 = {{2, 3}, {4, 6}};
  std::vector<mln::point2d> ref        = {
      {0, 0}, {0, 1}, {0, 2}, //
      {1, 0}, {1, 1}, {1, 2}, //
  };

  auto zipped_rows = mln::ranges::view::zip(multi_ind0, multi_ind1, multi_ind2);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(concepts::SegmentedRange<decltype(multi_ind0)>);
  static_assert(concepts::ReversibleRange<decltype(multi_ind0)>);
  static_assert(concepts::SegmentedRange<decltype(multi_ind1)>);
  static_assert(concepts::ReversibleRange<decltype(multi_ind1)>);
  static_assert(concepts::SegmentedRange<decltype(multi_ind2)>);
  static_assert(concepts::ReversibleRange<decltype(multi_ind2)>);
  static_assert(concepts::SegmentedRange<decltype(zipped_rows)>);
  static_assert(concepts::ReversibleRange<decltype(zipped_rows)>);
#else
  static_assert(::ranges::ForwardRange<decltype(multi_ind0)>());
  static_assert(::ranges::ForwardRange<decltype(multi_ind1)>());
  static_assert(::ranges::ForwardRange<decltype(multi_ind2)>());
  static_assert(::ranges::ForwardRange<decltype(zipped_rows)>());
#endif // PYLENE_CONCEPT_TS_ENABLED

  auto refv = std::begin(ref);
  for (auto&& row : zipped_rows.rows())
  {
    for (auto&& [v1, v2, v3] : row)
    {
      ASSERT_EQ(*refv, v1);
      ASSERT_EQ((*refv + mln::point2d{1, 2}), v2);
      ASSERT_EQ((*refv + mln::point2d{2, 3}), v3);
      refv++;
    }
  }
}

TEST(Core, zip_view_write)
{
  std::vector<int> a1 = {1, 2, 3, 6, 5, 4};
  std::vector<int> a2 = {1, 2, 3, 6, 5, 4};

  mln::ranges::multi_span<int, 2> sp1(a1.data(), {2, 3}, {3, 1});
  mln::ranges::multi_span<int, 2> sp2(a2.data(), {2, 3}, {3, 1});

  mln::box2d ind0 = {{1, 1}, {3, 4}};
  auto       z    = mln::ranges::view::zip(sp1, sp2, ind0);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(concepts::stl::ForwardRange<decltype(a1)>);
  static_assert(concepts::stl::ForwardRange<decltype(a2)>);
  static_assert(concepts::SegmentedRange<decltype(sp1)>);
  static_assert(concepts::ReversibleRange<decltype(sp1)>);
  static_assert(concepts::SegmentedRange<decltype(sp2)>);
  static_assert(concepts::ReversibleRange<decltype(sp2)>);
  static_assert(concepts::SegmentedRange<decltype(ind0)>);
  static_assert(concepts::ReversibleRange<decltype(ind0)>);
  static_assert(concepts::SegmentedRange<decltype(z)>);
  static_assert(concepts::ReversibleRange<decltype(z)>);
#else
  static_assert(::ranges::ForwardRange<decltype(a1)>());
  static_assert(::ranges::ForwardRange<decltype(a2)>());
  static_assert(::ranges::ForwardRange<decltype(sp1)>());
  static_assert(::ranges::ForwardRange<decltype(sp2)>());
  static_assert(::ranges::ForwardRange<decltype(ind0)>());
  static_assert(::ranges::ForwardRange<decltype(z)>());
#endif // PYLENE_CONCEPT_TS_ENABLED

  for (auto [x, y, p] : z)
  {
    x    = p[0]; // By ref
    y    = p[1]; // By ref
    p[0] = 42;   // No effect
  }

  std::vector<int> ref1 = {1, 1, 1, 2, 2, 2};
  std::vector<int> ref2 = {1, 2, 3, 1, 2, 3};
  EXPECT_EQ(ref1, a1);
  EXPECT_EQ(ref2, a2);
}

TEST(Core, zip_view_write_rowwise)
{
  std::vector<int> a1 = {1, 2, 3, 6, 5, 4};
  std::vector<int> a2 = {1, 2, 3, 6, 5, 4};

  mln::ranges::multi_span<int, 2> sp1(a1.data(), {2, 3}, {3, 1});
  mln::ranges::multi_span<int, 2> sp2(a2.data(), {2, 3}, {3, 1});

  mln::box2d ind0 = {{1, 1}, {3, 4}};
  auto       z    = mln::ranges::view::zip(sp1, sp2, ind0);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(concepts::stl::ForwardRange<decltype(a1)>);
  static_assert(concepts::stl::ForwardRange<decltype(a2)>);
  static_assert(concepts::SegmentedRange<decltype(sp1)>);
  static_assert(concepts::ReversibleRange<decltype(sp1)>);
  static_assert(concepts::SegmentedRange<decltype(sp2)>);
  static_assert(concepts::ReversibleRange<decltype(sp2)>);
  static_assert(concepts::SegmentedRange<decltype(ind0)>);
  static_assert(concepts::ReversibleRange<decltype(ind0)>);
  static_assert(concepts::SegmentedRange<decltype(z)>);
  static_assert(concepts::ReversibleRange<decltype(z)>);
#else
  static_assert(::ranges::ForwardRange<decltype(a1)>());
  static_assert(::ranges::ForwardRange<decltype(a2)>());
  static_assert(::ranges::ForwardRange<decltype(sp1)>());
  static_assert(::ranges::ForwardRange<decltype(sp2)>());
  static_assert(::ranges::ForwardRange<decltype(ind0)>());
  static_assert(::ranges::ForwardRange<decltype(z)>());
#endif // PYLENE_CONCEPT_TS_ENABLED

  for (auto&& r : z.rows())
  {
    for (auto [x, y, p] : r)
    {
      x    = p[0]; // By ref
      y    = p[1]; // By ref
      p[0] = 42;   // No effect
    }
  }

  std::vector<int> ref1 = {1, 1, 1, 2, 2, 2};
  std::vector<int> ref2 = {1, 2, 3, 1, 2, 3};
  EXPECT_EQ(ref1, a1);
  EXPECT_EQ(ref2, a2);
}


TEST(Core, zip_segmented_and_nonsegmented)
{
  mln::box2d multi_ind0 = {{0, 0}, {2, 3}};

  std::vector<mln::point2d> ref = {
      {0, 0}, {0, 1}, {0, 2}, //
      {1, 0}, {1, 1}, {1, 2}, //
  };

  auto z = mln::ranges::view::zip(ref, multi_ind0);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(concepts::SegmentedRange<decltype(multi_ind0)>);
  static_assert(concepts::ReversibleRange<decltype(multi_ind0)>);
  static_assert(concepts::stl::ForwardRange<decltype(z)>);
#else
  static_assert(::ranges::ForwardRange<decltype(multi_ind0)>());
  static_assert(::ranges::ForwardRange<decltype(z)>());
#endif // PYLENE_CONCEPT_TS_ENABLED

  for (auto [v1, v2] : z)
    ASSERT_EQ(v1, v2);
}


TEST(Core, zip_segmented_and_nonsegmented_rowwise)
{
  mln::box2d multi_ind0 = {{0, 0}, {2, 3}};

  std::vector<mln::point2d> ref = {
      {0, 0}, {0, 1}, {0, 2}, //
      {1, 0}, {1, 1}, {1, 2}, //
  };

  auto z = mln::ranges::view::zip(ref, multi_ind0);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(concepts::SegmentedRange<decltype(multi_ind0)>);
  static_assert(concepts::ReversibleRange<decltype(multi_ind0)>);
  static_assert(concepts::stl::ForwardRange<decltype(z)>);
#else
  static_assert(::ranges::ForwardRange<decltype(multi_ind0)>());
  static_assert(::ranges::ForwardRange<decltype(z)>());
#endif // PYLENE_CONCEPT_TS_ENABLED

  for (auto&& row : mln::ranges::rows(z))
    for (auto [v1, v2] : row)
      ASSERT_EQ(v1, v2);
}
