#include <mln/core/rangev3/multi_indices.hpp>
#include <mln/core/rangev3/multi_span.hpp>
#include <mln/core/rangev3/rows.hpp>
#include <mln/core/rangev3/view/transform.hpp>


#include <gtest/gtest.h>
#include <vector>

using vec_t = std::array<int, 2>;

std::size_t number_from_vec_t(vec_t x)
{
  return x[0] + x[1] * 100;
}

TEST(Range, transform_on_containers)
{
  std::vector<int> v   = {2, 3};
  auto             rng = mln::ranges::view::transform(v, [](int x) { return x * 2; });

  ASSERT_EQ(std::vector({4, 6}), ::ranges::to_vector(rng));
}


TEST(Range, transform_2d_readonly)
{
  mln::ranges::multi_indices<2> ind({3, 4});

  std::size_t n = 0;
  for (auto v : mln::ranges::view::transform(ind, number_from_vec_t))
    n += v;

  ASSERT_EQ(1812, n);
}


TEST(Range, transform_2d_readonly_rowwise)
{
  mln::ranges::multi_indices<2> ind({3, 4});

  std::size_t n   = 0;
  auto        rng = mln::ranges::view::transform(ind, number_from_vec_t);
  for (auto row : rng.rows())
    for (auto v : row)
      n += v;
  ASSERT_EQ(1812, n);
}


TEST(Range, transform_2d_write)
{
  std::vector<std::pair<int, int>>                buffer(12, std::make_pair(0, 0));
  mln::ranges::multi_span<std::pair<int, int>, 2> sp(buffer.data(), {3, 4}, {4, 1});

  {
    std::size_t i = 0;
    for (auto& v : mln::ranges::view::transform(sp, &std::pair<int, int>::first))
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

  {
    auto        rng = mln::ranges::view::transform(sp, &std::pair<int, int>::first);
    std::size_t i   = 0;
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
  std::vector<int> v = {2,3};
  auto rng = mln::ranges::view::transform(v, v, [](int x, int y) { return x + y; });

  ASSERT_EQ(std::vector({4,6}), ::ranges::to_vector(rng));
}



TEST(Range, transform2_2d_readonly)
{
  mln::ranges::multi_indices<2> ind({3,4});

  std::size_t n = 0;
  auto g = [](auto a, auto b) { return number_from_vec_t(a) + number_from_vec_t(b); };
  for (auto v : mln::ranges::view::transform(ind, ind, g))
    n += v;

  ASSERT_EQ(2 * 1812, n);
}


TEST(Range, transform2_2d_readonly_rowwise)
{
  mln::ranges::multi_indices<2> ind({3,4});

  std::size_t n = 0;
  auto g = [](auto a, auto b) { return number_from_vec_t(a) + number_from_vec_t(b); };
  auto rng = mln::ranges::view::transform(ind, ind, g);
  for (auto row : rng.rows())
    for (auto v : row)
      n += v;
  ASSERT_EQ(2 * 1812, n);
}


TEST(Range, transform2_2d_write)
{
  std::vector<std::pair<int,int>> buffer1(12, std::make_pair(-1,-1));
  std::vector<std::pair<int,int>> buffer2(12, std::make_pair(-2,-2));

  mln::ranges::multi_span<std::pair<int,int>, 2> sp1(buffer1.data(), {3,4}, {4,1});
  mln::ranges::multi_span<std::pair<int,int>, 2> sp2(buffer2.data(), {3,4}, {4,1});

  auto firsts_of_pairs = [](std::pair<int, int>& a, std::pair<int, int>& b) { return std::tie(a.first, b.first); };

  {
    std::size_t i = 0;
    for (auto&& v : mln::ranges::view::transform(sp1, sp2, firsts_of_pairs))
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
  std::vector<std::pair<int,int>> buffer1(12, std::make_pair(-1,-1));
  std::vector<std::pair<int,int>> buffer2(12, std::make_pair(-2,-2));

  mln::ranges::multi_span<std::pair<int,int>, 2> sp1(buffer1.data(), {3,4}, {4,1});
  mln::ranges::multi_span<std::pair<int,int>, 2> sp2(buffer2.data(), {3,4}, {4,1});

  auto firsts_of_pairs = [](std::pair<int, int>& a, std::pair<int, int>& b) { return std::tie(a.first, b.first); };

  {
    std::size_t i = 0;
    auto rng = mln::ranges::view::transform(sp1, sp2, firsts_of_pairs);
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

