#include <mln/core/rangev3/view/reverse.hpp>

#include <mln/core/rangev3/mdindex.hpp>

#include <range/v3/algorithm/copy.hpp>
#include <range/v3/iterator/insert_iterators.hpp>
#include <range/v3/range/conversion.hpp>

#include <vector>

#include <gtest/gtest.h>


TEST(Range, reversed_with_bidirectional_range)
{
  std::vector<int> v   = {1, 2, 3, 4, 5};
  std::vector<int> ref = {5, 4, 3, 2, 1};
  auto             res = mln::ranges::view::reverse(v);

  static_assert(::ranges::cpp20::bidirectional_range<decltype(v)>);
  static_assert(::ranges::cpp20::bidirectional_range<decltype(res)>);

  ASSERT_EQ(ref, ::ranges::to_vector(res));
}


TEST(Range, reversed_with_reversible_range)
{
  mln::ranges::mdindex<2>         ids({4, 3});
  std::vector<std::array<int, 2>> ref = {{3, 2}, {2, 2}, {1, 2}, {0, 2}, //
                                         {3, 1}, {2, 1}, {1, 1}, {0, 1}, //
                                         {3, 0}, {2, 0}, {1, 0}, {0, 0}};

  auto res = mln::ranges::view::reverse(ids);
  static_assert(mln::ranges::MDBidirectionalRange<decltype(res)>);

  std::vector<std::array<int, 2>> out;
  for (auto&& r : res.rows())
    ::ranges::copy(r, ::ranges::back_inserter(out));

  ASSERT_EQ(ref, out);
}

