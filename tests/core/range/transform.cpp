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


TEST(Range, transform2_on_containers)
{
  std::vector<int> v   = {2, 3};
  std::vector<int> v2  = {3, 4};
  auto             rng = mln::ranges::view::transform(v, v2, [](int x, int y) { return x + y; });

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::stl::ForwardRange<decltype(v)>);
  static_assert(mln::concepts::stl::ForwardRange<decltype(v2)>);
  static_assert(mln::concepts::stl::ForwardRange<decltype(rng)>);
#else
  static_assert(::ranges::ForwardRange<decltype(v)>());
  static_assert(::ranges::ForwardRange<decltype(v2)>());
  static_assert(::ranges::ForwardRange<decltype(rng)>());
#endif // PYLENE_CONCEPT_TS_ENABLED

  ASSERT_EQ(std::vector({5, 7}), ::ranges::to_vector(rng));
}


TEST(Range, transform2_2d_readonly)
{
  mln::ranges::multi_indices<2> ind({3, 4});

  std::size_t n   = 0;
  auto        g   = [](auto a, auto b) { return number_from_vec_t(a) + number_from_vec_t(b); };
  auto        rng = mln::ranges::view::transform(ind, ind, g);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::SegmentedRange<decltype(ind)>);
  static_assert(mln::concepts::ReversibleRange<decltype(ind)>);
  static_assert(mln::concepts::SegmentedRange<decltype(rng)>);
  static_assert(mln::concepts::ReversibleRange<decltype(rng)>);
#else
  static_assert(::ranges::ForwardRange<decltype(ind)>());
  static_assert(::ranges::ForwardRange<decltype(rng)>());
#endif // PYLENE_CONCEPT_TS_ENABLED

  for (auto v : rng)
    n += v;

  ASSERT_EQ(2 * 1812, n);
}


TEST(Range, transform2_2d_readonly_rowwise)
{
  mln::ranges::multi_indices<2> ind({3, 4});

  std::size_t n   = 0;
  auto        g   = [](auto a, auto b) { return number_from_vec_t(a) + number_from_vec_t(b); };
  auto        rng = mln::ranges::view::transform(ind, ind, g);

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
  ASSERT_EQ(2 * 1812, n);
}


TEST(Range, transform2_2d_write)
{
  std::vector<std::pair<int, int>> buffer1(12, std::make_pair(-1, -1));
  std::vector<std::pair<int, int>> buffer2(12, std::make_pair(-2, -2));

  mln::ranges::multi_span<std::pair<int, int>, 2> sp1(buffer1.data(), {3, 4}, {4, 1});
  mln::ranges::multi_span<std::pair<int, int>, 2> sp2(buffer2.data(), {3, 4}, {4, 1});

  auto firsts_of_pairs = [](std::pair<int, int>& a, std::pair<int, int>& b) { return std::tie(a.first, b.first); };
  auto rng             = mln::ranges::view::transform(sp1, sp2, firsts_of_pairs);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::stl::ForwardRange<decltype(buffer1)>);
  static_assert(mln::concepts::stl::ForwardRange<decltype(buffer2)>);
  static_assert(mln::concepts::SegmentedRange<decltype(sp1)>);
  static_assert(mln::concepts::ReversibleRange<decltype(sp1)>);
  static_assert(mln::concepts::SegmentedRange<decltype(sp2)>);
  static_assert(mln::concepts::ReversibleRange<decltype(sp2)>);
  static_assert(mln::concepts::SegmentedRange<decltype(rng)>);
  static_assert(mln::concepts::ReversibleRange<decltype(rng)>);
#else
  static_assert(::ranges::ForwardRange<decltype(buffer1)>());
  static_assert(::ranges::ForwardRange<decltype(buffer2)>());
  static_assert(::ranges::ForwardRange<decltype(sp1)>());
  static_assert(::ranges::ForwardRange<decltype(sp2)>());
  static_assert(::ranges::ForwardRange<decltype(rng)>());
#endif // PYLENE_CONCEPT_TS_ENABLED

  {
    std::size_t i = 0;
    for (auto&& v : rng)
    {
      std::get<0>(v) = i++;
      std::get<1>(v) = i++;
    }
  }

  for (int i = 0; i < 12; ++i)
  {
    ASSERT_EQ(2 * i, buffer1[i].first);
    ASSERT_EQ(2 * i + 1, buffer2[i].first);
    ASSERT_EQ(-1, buffer1[i].second);
    ASSERT_EQ(-2, buffer2[i].second);
  }
}

TEST(Range, transform2_2d_write_row_wise)
{
  std::vector<std::pair<int, int>> buffer1(12, std::make_pair(-1, -1));
  std::vector<std::pair<int, int>> buffer2(12, std::make_pair(-2, -2));

  mln::ranges::multi_span<std::pair<int, int>, 2> sp1(buffer1.data(), {3, 4}, {4, 1});
  mln::ranges::multi_span<std::pair<int, int>, 2> sp2(buffer2.data(), {3, 4}, {4, 1});

  auto firsts_of_pairs = [](std::pair<int, int>& a, std::pair<int, int>& b) { return std::tie(a.first, b.first); };
  auto rng             = mln::ranges::view::transform(sp1, sp2, firsts_of_pairs);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::stl::ForwardRange<decltype(buffer1)>);
  static_assert(mln::concepts::stl::ForwardRange<decltype(buffer2)>);
  static_assert(mln::concepts::SegmentedRange<decltype(sp1)>);
  static_assert(mln::concepts::ReversibleRange<decltype(sp1)>);
  static_assert(mln::concepts::SegmentedRange<decltype(sp2)>);
  static_assert(mln::concepts::ReversibleRange<decltype(sp2)>);
  static_assert(mln::concepts::SegmentedRange<decltype(rng)>);
  static_assert(mln::concepts::ReversibleRange<decltype(rng)>);
#else
  static_assert(::ranges::ForwardRange<decltype(buffer1)>());
  static_assert(::ranges::ForwardRange<decltype(buffer2)>());
  static_assert(::ranges::ForwardRange<decltype(sp1)>());
  static_assert(::ranges::ForwardRange<decltype(sp2)>());
  static_assert(::ranges::ForwardRange<decltype(rng)>());
#endif // PYLENE_CONCEPT_TS_ENABLED

  {
    std::size_t i = 0;
    for (auto row : rng.rows())
      for (auto&& v : row)
      {
        std::get<0>(v) = i++;
        std::get<1>(v) = i++;
      }
  }

  for (int i = 0; i < 12; ++i)
  {
    ASSERT_EQ(2 * i, buffer1[i].first);
    ASSERT_EQ(2 * i + 1, buffer2[i].first);
    ASSERT_EQ(-1, buffer1[i].second);
    ASSERT_EQ(-2, buffer2[i].second);
  }
}


TEST(Range, transform2_read_chain)
{
  std::vector<int> buffer1(12, -1);
  std::vector<int> buffer2(12, -2);

  mln::ranges::multi_span<int, 2> sp1(buffer1.data(), {3, 4}, {4, 1});
  mln::ranges::multi_span<int, 2> sp2(buffer2.data(), {3, 4}, {4, 1});

  [[maybe_unused]] auto x = mln::ranges::view::transform(sp1, [](int a) { return a > 3 ? a : 42; });
  [[maybe_unused]] auto y = mln::ranges::view::transform(x, sp1, [](int a, int b) { return a + b; });
  [[maybe_unused]] ::ranges::iterator_t<decltype(y)> it = ::ranges::begin(y);

  // FIXME: reproduce bug HERE (const above is important)
  [[maybe_unused]] const auto z = y;
  // [[maybe_unused]] ::ranges::iterator_t<decltype(z)> it = ::ranges::begin(z);

  // [[maybe_unused]] auto rng = mln::ranges::view::transform(y, sp2, [](int a, int b) { return a * b; });

  // static_assert(mln::concepts::stl::ForwardRange<decltype(rng)>);
}
