#include <mln/core/rangev3/multi_indices.hpp>
#include <mln/core/rangev3/rows.hpp>
#include <mln/core/rangev3/view/filter.hpp>
#include <mln/core/rangev3/view/reverse.hpp>
#include <mln/core/rangev3/view/transform.hpp>
#include <mln/core/rangev3/view/zip.hpp>

#include <mln/core/concept/new/concepts.hpp>

#include <range/v3/to_container.hpp>
#include <range/v3/view/single.hpp>

#include <vector>

#include <gtest/gtest.h>


TEST(Range, reversed_with_bidirectional_range)
{
  std::vector<int> v   = {1, 2, 3, 4, 5};
  std::vector<int> ref = {5, 4, 3, 2, 1};
  auto             res = mln::ranges::view::reverse(v);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::stl::ForwardRange<decltype(v)>);
  static_assert(mln::concepts::stl::ForwardRange<decltype(res)>);
#else
  static_assert(::ranges::ForwardRange<decltype(v)>());
  static_assert(::ranges::ForwardRange<decltype(res)>());
#endif // PYLENE_CONCEPT_TS_ENABLED

  ASSERT_EQ(ref, ::ranges::to_vector(res));
}


TEST(Range, reversed_with_reversible_range)
{
  mln::ranges::multi_indices<2>   ids({3, 4});
  std::vector<std::array<int, 2>> ref = {{2, 3}, {2, 2}, {2, 1}, {2, 0}, //
                                         {1, 3}, {1, 2}, {1, 1}, {1, 0}, //
                                         {0, 3}, {0, 2}, {0, 1}, {0, 0}};

  auto res = mln::ranges::view::reverse(ids);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::SegmentedRange<decltype(ids)>);
  static_assert(mln::concepts::ReversibleRange<decltype(ids)>);
  static_assert(mln::concepts::SegmentedRange<decltype(res)>);
  static_assert(mln::concepts::ReversibleRange<decltype(res)>);
#else
  static_assert(::ranges::ForwardRange<decltype(ids)>());
  static_assert(::ranges::ForwardRange<decltype(res)>());
#endif // PYLENE_CONCEPT_TS_ENABLED

  ASSERT_EQ(ref, ::ranges::to_vector(res));
}

TEST(Range, reversed_filtered_bidirectional_range)
{
  std::vector<int> v    = {1, 2, 3, 4, 5};
  std::vector<int> ref  = {4, 2};
  auto             even = mln::ranges::view::filter(v, [](auto val) { return val % 2 == 0; });
  auto             res  = mln::ranges::view::reverse(even);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::stl::ForwardRange<decltype(v)>);
  static_assert(mln::concepts::stl::ForwardRange<decltype(res)>);
#else
  static_assert(::ranges::ForwardRange<decltype(v)>());
  static_assert(::ranges::ForwardRange<decltype(res)>());
#endif // PYLENE_CONCEPT_TS_ENABLED

  ASSERT_EQ(ref, ::ranges::to_vector(res));
}

TEST(Range, reversed_filtered_reversible_range)
{
  mln::ranges::multi_indices<2>   ids({3, 4});
  std::vector<std::array<int, 2>> ref = {{2, 2}, {2, 0}, {1, 3}, {1, 1}, {0, 2}, {0, 0}};
  auto even_sum = mln::ranges::view::filter(ids, [](auto vals) { return (vals[0] + vals[1]) % 2 == 0; });
  auto res      = mln::ranges::view::reverse(even_sum);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::SegmentedRange<decltype(ids)>);
  static_assert(mln::concepts::ReversibleRange<decltype(ids)>);
  static_assert(mln::concepts::SegmentedRange<decltype(res)>);
  static_assert(mln::concepts::ReversibleRange<decltype(res)>);
#else
  static_assert(::ranges::ForwardRange<decltype(ids)>());
  static_assert(::ranges::ForwardRange<decltype(res)>());
#endif // PYLENE_CONCEPT_TS_ENABLED

  ASSERT_EQ(ref, ::ranges::to_vector(res));
}


TEST(Range, reversed_transformed_bidirectional_range)
{
  std::vector<int> v    = {1, 2, 3, 4, 5};
  std::vector<int> ref  = {25, 16, 9, 4, 1};
  auto             pow2 = mln::ranges::view::transform(v, [](auto val) { return val * val; });
  auto             res  = mln::ranges::view::reverse(pow2);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::stl::ForwardRange<decltype(v)>);
  static_assert(mln::concepts::stl::ForwardRange<decltype(res)>);
#else
  static_assert(::ranges::ForwardRange<decltype(v)>());
  static_assert(::ranges::ForwardRange<decltype(res)>());
#endif // PYLENE_CONCEPT_TS_ENABLED

  ASSERT_EQ(ref, ::ranges::to_vector(res));
}

TEST(Range, reversed_transformed_reversible_range)
{
  mln::ranges::multi_indices<2>   ids({3, 4});
  std::vector<std::array<int, 2>> ref  = {{4, 9}, {4, 4}, {4, 1}, {4, 0}, //
                                         {1, 9}, {1, 4}, {1, 1}, {1, 0}, //
                                         {0, 9}, {0, 4}, {0, 1}, {0, 0}};
  auto                            pow2 = mln::ranges::view::transform(ids, [](auto vals) {
    return std::array<int, 2>{vals[0] * vals[0], vals[1] * vals[1]};
  });
  auto                            res  = mln::ranges::view::reverse(pow2);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::SegmentedRange<decltype(ids)>);
  static_assert(mln::concepts::ReversibleRange<decltype(ids)>);
  static_assert(mln::concepts::SegmentedRange<decltype(res)>);
  static_assert(mln::concepts::ReversibleRange<decltype(res)>);
#else
  static_assert(::ranges::ForwardRange<decltype(ids)>());
  static_assert(::ranges::ForwardRange<decltype(res)>());
#endif // PYLENE_CONCEPT_TS_ENABLED

  ASSERT_EQ(ref, ::ranges::to_vector(res));
}

TEST(Range, zipped_reversed_range)
{
  std::vector<int>                  v   = {1, 2, 3, 4, 5};
  std::vector<int>                  w   = {10, 20, 30, 40, 50};
  std::vector<std::tuple<int, int>> ref = {{5, 50}, {4, 40}, {3, 30}, {2, 20}, {1, 10}};

  auto tmp     = mln::ranges::view::zip(mln::ranges::view::reverse(v), mln::ranges::view::reverse(w));
  auto tmp_vec = ::ranges::to_vector(tmp);
  std::vector<std::tuple<int, int>> res = {tmp_vec.begin(), tmp_vec.end()};

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::stl::ForwardRange<decltype(v)>);
  static_assert(mln::concepts::stl::ForwardRange<decltype(w)>);
  static_assert(mln::concepts::stl::ForwardRange<decltype(res)>);
#else
  static_assert(::ranges::ForwardRange<decltype(v)>());
  static_assert(::ranges::ForwardRange<decltype(w)>());
  static_assert(::ranges::ForwardRange<decltype(res)>());
#endif // PYLENE_CONCEPT_TS_ENABLED

  ASSERT_EQ(ref, res);
}

TEST(Core, reversed_zipped_container)
{
  std::vector<int>                  v   = {1, 2, 3, 4, 5};
  std::vector<int>                  w   = {10, 20, 30, 40, 50};
  std::vector<std::tuple<int, int>> ref = {{5, 50}, {4, 40}, {3, 30}, {2, 20}, {1, 10}};

  auto                              tmp     = mln::ranges::view::reverse(mln::ranges::view::zip(v, w));
  auto                              tmp_vec = ::ranges::to_vector(tmp);
  std::vector<std::tuple<int, int>> res     = {tmp_vec.begin(), tmp_vec.end()};

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::stl::ForwardRange<decltype(v)>);
  static_assert(mln::concepts::stl::ForwardRange<decltype(w)>);
  static_assert(mln::concepts::stl::ForwardRange<decltype(res)>);
#else
  static_assert(::ranges::ForwardRange<decltype(v)>());
  static_assert(::ranges::ForwardRange<decltype(w)>());
  static_assert(::ranges::ForwardRange<decltype(res)>());
#endif // PYLENE_CONCEPT_TS_ENABLED

  ASSERT_EQ(ref, res);
}
