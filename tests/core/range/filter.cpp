#include <mln/core/range/view/filter.hpp>

#include <mln/core/box.hpp>
#include <mln/core/range/concepts.hpp>
#include <mln/core/range/mdspan.hpp>
#include <mln/core/range/rows.hpp>

#include <algorithm>
#include <numeric>

#include <gtest/gtest.h>


TEST(Range, filter_2d_readonly)
{
  std::vector<mln::experimental::point2d> ref = {{0, 0}, {2, 0}, {1, 1}, {3, 1}, {0, 2}, {2, 2}};
  mln::experimental::box2d                box(4, 3);

  auto chessboard_pred = [](auto p) { return (p[0] % 2) == (p[1] % 2); };
  auto rng             = mln::ranges::view::filter(box, chessboard_pred);

  std::vector<mln::experimental::point2d> res;
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
  auto sp  = mln::ranges::mdspan<int, 2>(buffer.data(), {4, 3});
  auto rng = mln::ranges::view::filter(sp, [](int val) { return val % 2 == 0; });

  static_assert(mln::ranges::MDRange<decltype(rng)>);
  static_assert(mln::ranges::output_mdrange<decltype(rng), int>);


  for (auto row : rng.rows())
    for (int& val : row)
      val = 42;

  ASSERT_EQ(ref, buffer);
}

/* FIXME
TEST(Range, reversed_filtered_bidirectional_range)
{
  std::vector<int> v    = {1, 2, 3, 4, 5};
  std::vector<int> ref  = {4, 2};
  auto             even = mln::ranges::view::filter(v, [](auto val) { return val % 2 == 0; });
  auto             res  = mln::ranges::view::reverse(even);

  static_assert(mln::concepts::stl::ForwardRange<decltype(v)>);
  static_assert(mln::concepts::stl::ForwardRange<decltype(res)>);
#else
  static_assert(::ranges::ForwardRange<decltype(v)>());
  static_assert(::ranges::ForwardRange<decltype(res)>());

  ASSERT_EQ(ref, ::ranges::to_vector(res));
}

TEST(Range, reversed_filtered_reversible_range)
{
  mln::ranges::multi_indices<2>   ids({3, 4});
  std::vector<std::array<int, 2>> ref = {{2, 2}, {2, 0}, {1, 3}, {1, 1}, {0, 2}, {0, 0}};
  auto even_sum = mln::ranges::view::filter(ids, [](auto vals) { return (vals[0] + vals[1]) % 2 == 0; });
  auto res      = mln::ranges::view::reverse(even_sum);

  static_assert(mln::concepts::SegmentedRange<decltype(ids)>);
  static_assert(mln::concepts::ReversibleRange<decltype(ids)>);
  static_assert(mln::concepts::SegmentedRange<decltype(res)>);
  static_assert(mln::concepts::ReversibleRange<decltype(res)>);
#else
  static_assert(::ranges::ForwardRange<decltype(ids)>());
  static_assert(::ranges::ForwardRange<decltype(res)>());

  ASSERT_EQ(ref, ::ranges::to_vector(res));
}


*/
