#include <mln/core/rangev3/multi_indices.hpp>
#include <mln/core/rangev3/view/reverse.hpp>
#include <range/v3/to_container.hpp>

#include <gtest/gtest.h>
#include <vector>



TEST(Range, reversed_with_bidirectional_range)
{
  std::vector<int> v = {1,2,3,4,5};
  std::vector<int> ref = {5,4,3,2,1};
  auto res = mln::ranges::view::reverse(v);
  ASSERT_EQ(ref, ::ranges::to_vector(res));
}


TEST(Range, reversed_with_reversible_range)
{
  mln::ranges::multi_indices<2> ids({3,4});
  std::vector<std::array<int,2>> ref = {
    {2,3}, {2,2}, {2,1}, {2,0},
    {1,3}, {1,2}, {1,1}, {1,0},
    {0,3}, {0,2}, {0,1}, {0,0}
  };

  auto res = mln::ranges::view::reverse(ids);
  ASSERT_EQ(ref, ::ranges::to_vector(res));
}
