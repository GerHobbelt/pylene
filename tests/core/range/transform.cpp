#include <mln/core/rangev3/multi_indices.hpp>
#include <mln/core/rangev3/multi_span.hpp>
#include <mln/core/rangev3/rows.hpp>
#include <mln/core/rangev3/view/transform.hpp>

#include <mln/core/concept/new/concepts.hpp>

#include <vector>

#include <gtest/gtest.h>


using vec_t = std::array<int, 2>;

std::size_t number_from_vec_t(vec_t x)
{
  return x[0] + x[1] * 100;
}

TEST(Range, transform_on_containers)
{
  std::vector<int> v   = {2, 3};
  auto             rng = mln::ranges::view::transform(v, [](int x) { return x * 2; });

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::stl::ForwardRange<decltype(v)>);
  static_assert(mln::concepts::stl::ForwardRange<decltype(rng)>);
#else
  static_assert(::ranges::ForwardRange<decltype(v)>());
  static_assert(::ranges::ForwardRange<decltype(rng)>());
#endif // PYLENE_CONCEPT_TS_ENABLED

  ASSERT_EQ(std::vector({4, 6}), ::ranges::to_vector(rng));
}


TEST(Range, transform_2d_readonly)
{
  mln::ranges::multi_indices<2> ind({3, 4});
  auto                          rng = mln::ranges::view::transform(ind, number_from_vec_t);

  std::size_t n = 0;
  for (auto v : rng)
    n += v;

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::SegmentedRange<decltype(ind)>);
  static_assert(mln::concepts::ReversibleRange<decltype(ind)>);
  static_assert(mln::concepts::SegmentedRange<decltype(rng)>);
  static_assert(mln::concepts::ReversibleRange<decltype(rng)>);
#else
  static_assert(::ranges::ForwardRange<decltype(ind)>());
  static_assert(::ranges::ForwardRange<decltype(rng)>());
#endif // PYLENE_CONCEPT_TS_ENABLED

  ASSERT_EQ(1812, n);
}


TEST(Range, transform_2d_readonly_rowwise)
{
  mln::ranges::multi_indices<2> ind({3, 4});

  std::size_t n   = 0;
  auto        rng = mln::ranges::view::transform(ind, number_from_vec_t);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::SegmentedRange<decltype(ind)>);
  static_assert(mln::concepts::ReversibleRange<decltype(ind)>);
  static_assert(mln::concepts::SegmentedRange<decltype(rng)>);
  static_assert(mln::concepts::ReversibleRange<decltype(rng)>);
#else
  static_assert(::ranges::ForwardRange<decltype(ind)>());
  static_assert(::ranges::ForwardRange<decltype(rng)>());
#endif // PYLENE_CONCEPT_TS_ENABLED

  for (auto row : rng.rows())
    for (auto v : row)
      n += v;
  ASSERT_EQ(1812, n);
}


TEST(Range, transform_2d_write)
{
  std::vector<std::pair<int, int>>                buffer(12, std::make_pair(0, 0));
  mln::ranges::multi_span<std::pair<int, int>, 2> sp(buffer.data(), {3, 4}, {4, 1});
  auto                                            rng = mln::ranges::view::transform(sp, &std::pair<int, int>::first);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::stl::ForwardRange<decltype(buffer)>);
  static_assert(mln::concepts::SegmentedRange<decltype(sp)>);
  static_assert(mln::concepts::ReversibleRange<decltype(sp)>);
  static_assert(mln::concepts::SegmentedRange<decltype(rng)>);
  static_assert(mln::concepts::ReversibleRange<decltype(rng)>);
#else
  static_assert(::ranges::ForwardRange<decltype(buffer)>());
  static_assert(::ranges::ForwardRange<decltype(sp)>());
  static_assert(::ranges::ForwardRange<decltype(rng)>());
#endif // PYLENE_CONCEPT_TS_ENABLED

  {
    std::size_t i = 0;
    for (auto& v : rng)
      v = i++;
  }

  for (int i = 0; i < 12; ++i)
  {
    ASSERT_EQ(i, buffer[i].first);
    ASSERT_EQ(0, buffer[i].second);
  }
}


TEST(Range, transform_2d_write_row_wise)
{
  std::vector<std::pair<int, int>>                buffer(12, std::make_pair(0, 0));
  mln::ranges::multi_span<std::pair<int, int>, 2> sp(buffer.data(), {3, 4}, {4, 1});
  auto                                            rng = mln::ranges::view::transform(sp, &std::pair<int, int>::first);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::stl::ForwardRange<decltype(buffer)>);
  static_assert(mln::concepts::SegmentedRange<decltype(sp)>);
  static_assert(mln::concepts::ReversibleRange<decltype(sp)>);
  static_assert(mln::concepts::SegmentedRange<decltype(rng)>);
  static_assert(mln::concepts::ReversibleRange<decltype(rng)>);
#else
  static_assert(::ranges::ForwardRange<decltype(buffer)>());
  static_assert(::ranges::ForwardRange<decltype(sp)>());
  static_assert(::ranges::ForwardRange<decltype(rng)>());
#endif // PYLENE_CONCEPT_TS_ENABLED

  {
    std::size_t i = 0;
    for (auto row : rng.rows())
      for (int& v : row)
        v = i++;
  }

  for (int i = 0; i < 12; ++i)
  {
    ASSERT_EQ(i, buffer[i].first);
    ASSERT_EQ(0, buffer[i].second);
  }
}
