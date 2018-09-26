#include <mln/core/rangev3/foreach.hpp>
#include <mln/core/rangev3/multi_indices.hpp>
#include <vector>
#include <gtest/gtest.h>


using multi_indices = mln::ranges::multi_indices<2, int>;
using value_t = std::array<int, 2>;

TEST(Range, foreach_equal_twofors)
{
  std::vector<value_t> a, b;

  int width = 5;
  int height = 7;

  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      a.push_back({y,x});


  multi_indices sp(value_t{height, width});

  mln_foreach_new(auto v, sp)
    b.push_back(v);

  ASSERT_EQ(a, b);
}

TEST(Range, for_equal_twofors)
{
  std::vector<value_t> a, b;

  int width = 5;
  int height = 7;

  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
      a.push_back({y, x});

  multi_indices sp({height, width});
  for(auto v : sp)
    b.push_back(v);

  ASSERT_EQ(a, b);
}

TEST(Range, foreach_breaks)
{
  std::vector<value_t> a, b;

  int width = 5;
  int height = 7;

  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
    {
      if (x == 1 && y == 3)
        goto endfor;
      a.push_back({y, x});
    }

endfor:
  multi_indices sp({height, width});

  mln_foreach_new(auto v, sp)
  {
    if (v[1] == 1 && v[0] == 3)
      break;
    b.push_back(v);
  }

  ASSERT_EQ(a, b);
}

TEST(Range, foreach_continues)
{
  std::vector<value_t> a, b;

  int width = 5;
  int height = 7;

  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
    {
      if (x % 2 == 0 && y % 2 == 0)
        continue;
      a.push_back({y,x});
    }


  multi_indices sp({height, width});

  mln_foreach_new(auto v, sp)
  {
    if (v[0] % 2 == 0 && v[1] % 2 == 0)
      continue;
    b.push_back(v);
  }

  ASSERT_EQ(a, b);
}

