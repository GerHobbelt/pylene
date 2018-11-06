#include <mln/core/rangev3/multi_indices.hpp>
#include <mln/core/rangev3/rows.hpp>
#include <mln/core/rangev3/view/filter.hpp>
#include <mln/core/rangev3/view/reverse.hpp>
#include <mln/core/rangev3/view/transform.hpp>
#include <mln/core/rangev3/view/zip.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/single.hpp>

#include <iostream>
#include <vector>

#include <gtest/gtest.h>


TEST(Range, reversed_with_bidirectional_range)
{
  std::vector<int> v   = {1, 2, 3, 4, 5};
  std::vector<int> ref = {5, 4, 3, 2, 1};
  auto             res = mln::ranges::view::reverse(v);
  ASSERT_EQ(ref, ::ranges::to_vector(res));
}


TEST(Range, reversed_with_reversible_range)
{
  mln::ranges::multi_indices<2>   ids({3, 4});
  std::vector<std::array<int, 2>> ref = {{2, 3}, {2, 2}, {2, 1}, {2, 0}, {1, 3}, {1, 2},
                                         {1, 1}, {1, 0}, {0, 3}, {0, 2}, {0, 1}, {0, 0}};

  auto res = mln::ranges::view::reverse(ids);
  ASSERT_EQ(ref, ::ranges::to_vector(res));
}

TEST(Range, reversed_filtered_bidirectional_range)
{
  std::vector<int> v    = {1, 2, 3, 4, 5};
  std::vector<int> ref  = {4, 2};
  auto             even = mln::ranges::view::filter(v, [](auto val) { return val % 2 == 0; });
  auto             res  = mln::ranges::view::reverse(even);
  ASSERT_EQ(ref, ::ranges::to_vector(res));
}

// FIXME: not working... auto res = reverse(even_sum)
// "Cannot get a view of a temporary container" static_assert(std::is_lvalue_reference<T>::value)"
/*
TEST(Range, reversed_filtered_reversible_range)
{
  mln::ranges::multi_indices<2>   ids({3, 4});
  std::vector<std::array<int, 2>> ref = {{0, 0}, {0, 2}, {1, 1}, {2, 2}, {1, 3}, {2, 0}};
  auto even_sum = mln::ranges::view::filter(ids, [](auto vals) { return (vals[0] + vals[1]) % 2 == 0; });
  auto res      = mln::ranges::view::reverse(even_sum);
  ASSERT_EQ(ref, ::ranges::to_vector(res));
}
*/


TEST(Range, reversed_transformed_bidirectional_range)
{
  std::vector<int> v    = {1, 2, 3, 4, 5};
  std::vector<int> ref  = {25, 16, 9, 4, 1};
  auto             pow2 = mln::ranges::view::transform(v, [](auto val) { return val * val; });
  auto             res  = mln::ranges::view::reverse(pow2);
  ASSERT_EQ(ref, ::ranges::to_vector(res));
}

// FIXME: not working
/*
TEST(Range, reversed_transformed_reversible_range)
{
  mln::ranges::multi_indices<2>   ids({3, 4});
  std::vector<std::array<int, 2>> ref  = {{0, 0}, {0, 4}, {1, 1}, {4, 4}, {1, 9}, {4, 0}};
  auto                            pow2 = mln::ranges::view::transform(ids, [](auto vals) -> std::array<int, 2> {
    return {vals[0] * vals[0], vals[1] * vals[1]};
  });
  auto                            res  = mln::ranges::view::reverse(pow2);
  ASSERT_EQ(ref, ::ranges::to_vector(res));
}
*/

// FIXME: not working (see static_assert failing)
/*
TEST(Range, reversed_zipped_range)
{
  std::vector<int> v = {1, 2, 3, 4, 5};
  std::vector<int> w = {10, 20, 30, 40, 50};
  using _            = std::tuple<int, int>;
  std::vector<_> ref = {_{5, 50}, _{4, 40}, _{3, 30}, _{2, 20}, _{1, 10}};
  auto           z   = mln::ranges::view::zip_with([](auto a, auto b) { return std::make_tuple(a, b); }, v, w);
  static_assert(
      std::is_base_of_v<::ranges::bidirectional_iterator_tag, ::ranges::range_category_t<std::decay_t<decltype(z)>>>,
      "Not a bidirectional range");
  auto res = mln::ranges::view::reverse(z);
  ASSERT_EQ(ref, ::ranges::to_vector(res));
}
*/
