#include <mln/core/domain/box.hpp>
#include <mln/core/forall.hpp>
#include <mln/core/range/filter.hpp>

#include <mln/core/rangev3/multi_span.hpp>
#include <mln/core/rangev3/rows.hpp>
#include <mln/core/rangev3/view/filter.hpp>

#include <mln/core/concept/new/concepts.hpp>

#include <algorithm>
#include <numeric>

#include <gtest/gtest.h>


TEST(Core, range_filter)
{

  mln::box2d a({0, 0}, {6, 11});
  auto       x = mln::rng::filter(a, [](mln::point2d p) { return (p[0] % 2) == (p[1] % 2); }); // chess board

  unsigned sz = 0;
  mln_foreach (mln::point2d p, x)
  {
    (void)p;
    ++sz;
  }

  ASSERT_TRUE(x.has(mln::point2d{0, 0}));
  ASSERT_TRUE(x.has(mln::point2d{1, 1}));
  ASSERT_TRUE(not x.has(mln::point2d{0, 1}));
  ASSERT_TRUE(not x.has(mln::point2d{1, 0}));
  ASSERT_EQ(sz, 33u);
}


TEST(Range, filter_2d_readonly)
{
  std::vector<mln::point2d> ref = {{0, 0}, {0, 2}, {1, 1}, {1, 3}, {2, 0}, {2, 2}};
  mln::box2d                box = {{0, 0}, {3, 4}};

  auto chessboard_pred = [](auto p) { return (p[0] % 2) == (p[1] % 2); };
  auto rng             = mln::ranges::view::filter(box, chessboard_pred);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::SegmentedRange<decltype(box)>);
  static_assert(mln::concepts::ReversibleRange<decltype(box)>);
  static_assert(mln::concepts::SegmentedRange<decltype(rng)>);
  static_assert(mln::concepts::ReversibleRange<decltype(rng)>);
#else
  static_assert(::ranges::ForwardRange<decltype(box)>());
  static_assert(::ranges::ForwardRange<decltype(rng)>());
#endif // PYLENE_CONCEPT_TS_ENABLED

  std::vector<mln::point2d> res;
  for (auto p : rng)
    res.push_back(p);

  ASSERT_EQ(ref, res);
}

TEST(Range, filter_2d_readonly_rowwise)
{
  std::vector<mln::point2d> ref = {{0, 0}, {0, 2}, {1, 1}, {1, 3}, {2, 0}, {2, 2}};
  mln::box2d                box = {{0, 0}, {3, 4}};

  auto chessboard_pred = [](auto p) { return (p[0] % 2) == (p[1] % 2); };
  auto rng             = mln::ranges::view::filter(box, chessboard_pred);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::SegmentedRange<decltype(box)>);
  static_assert(mln::concepts::ReversibleRange<decltype(box)>);
  static_assert(mln::concepts::SegmentedRange<decltype(rng)>);
  static_assert(mln::concepts::ReversibleRange<decltype(rng)>);
#else
  static_assert(::ranges::ForwardRange<decltype(box)>());
  static_assert(::ranges::ForwardRange<decltype(rng)>());
#endif // PYLENE_CONCEPT_TS_ENABLED

  std::vector<mln::point2d> res;
  for (auto row : rng.rows())
    for (auto p : row)
      res.push_back(p);

  ASSERT_EQ(ref, res);
}

TEST(Range, filter_2d_write)
{
  std::vector<int> buffer(12, 0);
  std::iota(buffer.begin(), buffer.end(), 0);

  // make ref
  std::vector<int> ref = buffer;
  for (int i = 0; i < 12; i += 2)
    ref[i] = 42;


  // make res
  auto sp  = mln::ranges::multi_span<int, 2>{buffer.data(), {3, 4}, {4, 1}};
  auto rng = mln::ranges::view::filter(sp, [](int val) { return val % 2 == 0; });

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::SegmentedRange<decltype(sp)>);
  static_assert(mln::concepts::ReversibleRange<decltype(sp)>);
  static_assert(mln::concepts::stl::OutputRange<decltype(sp), int>);
  static_assert(mln::concepts::SegmentedRange<decltype(rng)>);
  static_assert(mln::concepts::ReversibleRange<decltype(rng)>);
  static_assert(mln::concepts::stl::OutputRange<decltype(rng), int>);
#else
  static_assert(::ranges::ForwardRange<decltype(sp)>());
  static_assert(mln::concepts::stl::OutputRange<decltype(sp), int>);
  static_assert(::ranges::ForwardRange<decltype(rng)>());
  static_assert(mln::concepts::stl::OutputRange<decltype(rng), int>);
#endif // PYLENE_CONCEPT_TS_ENABLED

  for (auto& val : rng)
    val = 42;

  ASSERT_EQ(ref, buffer);
}


TEST(Range, filter_2d_write_rowwise)
{
  std::vector<int> buffer(12, 0);
  std::iota(buffer.begin(), buffer.end(), 0);

  // make ref
  std::vector<int> ref = buffer;
  for (int i = 0; i < 12; i += 2)
    ref[i] = 42;


  // make res
  auto sp  = mln::ranges::multi_span<int, 2>{buffer.data(), {3, 4}, {4, 1}};
  auto rng = mln::ranges::view::filter(sp, [](int val) { return val % 2 == 0; });

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::SegmentedRange<decltype(sp)>);
  static_assert(mln::concepts::ReversibleRange<decltype(sp)>);
  static_assert(mln::concepts::stl::OutputRange<decltype(sp), int>);
  static_assert(mln::concepts::SegmentedRange<decltype(rng)>);
  static_assert(mln::concepts::ReversibleRange<decltype(rng)>);
  static_assert(mln::concepts::stl::OutputRange<decltype(rng), int>);
#else
  static_assert(::ranges::ForwardRange<decltype(sp)>());
  static_assert(mln::concepts::stl::OutputRange<decltype(sp), int>);
  static_assert(::ranges::ForwardRange<decltype(rng)>());
  static_assert(mln::concepts::stl::OutputRange<decltype(rng), int>);
#endif // PYLENE_CONCEPT_TS_ENABLED

  for (auto row : rng.rows())
    for (int& val : row)
      val = 42;

  ASSERT_EQ(ref, buffer);
}
