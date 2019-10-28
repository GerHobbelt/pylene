#include <mln/core/rangev3/view/transform.hpp>

#include <mln/core/rangev3/algorithm/equal.hpp>
#include <mln/core/rangev3/mdindex.hpp>
#include <mln/core/rangev3/mdspan.hpp>
#include <mln/core/rangev3/view/reverse.hpp>

#include <range/v3/iterator/insert_iterators.hpp>
#include <range/v3/algorithm/copy.hpp>

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


  static_assert(::ranges::cpp20::forward_range<decltype(v)>);
  static_assert(::ranges::cpp20::forward_range<decltype(rng)>);
  ASSERT_EQ(std::vector({4, 6}), ::ranges::to_vector(rng));
}


TEST(Range, transform_2d_readonly)
{
  mln::ranges::mdindex<2> ind({3, 4});

  std::size_t n   = 0;
  auto        rng = mln::ranges::view::transform(ind, number_from_vec_t);

  static_assert(mln::ranges::MDBidirectionalRange<decltype(rng)>);

  for (auto row : rng.rows())
    for (auto v : row)
      n += v;
  ASSERT_EQ(1812, n);
}


TEST(Range, transform_2d_write)
{
  std::vector<std::pair<int, int>>                buffer(12, std::make_pair(0, 0));

  std::size_t    dims[]    = {4, 3};
  std::ptrdiff_t strides[] = {1, 4};

  auto sp = mln::ranges::mdspan<std::pair<int, int>, 2>(buffer.data(), dims, strides);
  auto rng = mln::ranges::view::transform(sp, &std::pair<int, int>::first);
  static_assert(mln::ranges::MDBidirectionalRange<decltype(sp)>);
  static_assert(mln::ranges::MDBidirectionalRange<decltype(rng)>);

  {
    std::size_t i = 0;
    for (auto&& r : rng.rows())
      for (int& v : r)
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

  ASSERT_EQ(std::vector({5, 7}), ::ranges::to_vector(rng));
}


TEST(Range, transform2_2d_readonly)
{
  mln::ranges::mdindex<2> ind({3, 4});

  std::size_t n   = 0;
  auto        g   = [](auto a, auto b) { return number_from_vec_t(a) + number_from_vec_t(b); };
  auto        rng = mln::ranges::view::transform(ind, ind, g);

  static_assert(mln::ranges::MDBidirectionalRange<decltype(rng)>);

  for (auto row : rng.rows())
    for (auto v : row)
      n += v;
  ASSERT_EQ(2 * 1812, n);
}


TEST(Range, transform2_2d_write_row_wise)
{
  std::vector<std::pair<int, int>> buffer1(12, std::make_pair(-1, -1));
  std::vector<std::pair<int, int>> buffer2(12, std::make_pair(-2, -2));

  mln::ranges::mdspan<std::pair<int, int>, 2> sp1(buffer1.data(), {4, 3}, {1, 4});
  mln::ranges::mdspan<std::pair<int, int>, 2> sp2(buffer2.data(), {4, 3}, {1, 4});

  auto firsts_of_pairs = [](std::pair<int, int>& a, std::pair<int, int>& b) { return std::tie(a.first, b.first); };
  auto rng             = mln::ranges::view::transform(sp1, sp2, firsts_of_pairs);

  static_assert(mln::ranges::MDBidirectionalRange<decltype(sp1)>);
  static_assert(mln::ranges::MDBidirectionalRange<decltype(sp2)>);
  static_assert(mln::ranges::MDBidirectionalRange<decltype(rng)>);

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

TEST(Range, transform_read_chain)
{
  std::vector<int> buffer(12, -1);

  mln::ranges::mdspan<int, 2> sp(buffer.data(), {4, 3}, {1, 4});

  auto x = mln::ranges::view::transform(sp, [](int a) { return a > 3 ? a : 42; });
  auto y = mln::ranges::view::transform(x, [](int a) { return a + 1; });
  auto z = mln::ranges::view::transform(y, [](int a) { return a * 2; });

  std::vector<int>                buffer_ref(12, 86);
  mln::ranges::mdspan<int, 2>     ref(buffer_ref.data(), {4, 3}, {1, 4});

  ASSERT_TRUE(mln::ranges::equal(z, ref));
}

TEST(Range, transform2_read_chain)
{
  std::vector<int> buffer(12, -1);

  mln::ranges::mdspan<int, 2> sp(buffer.data(), {4, 3}, {1, 4});

  auto x = mln::ranges::view::transform(sp, [](int a) { return a > 3 ? a : 42; });
  auto y = mln::ranges::view::transform(x, sp, [](int a, int b) { return a + b; });
  auto z = mln::ranges::view::transform(y, [](int a) { return a + 1; });

  std::vector<int>                buffer_ref(12, 42);
  mln::ranges::mdspan<int, 2> ref(buffer_ref.data(), {4, 3}, {1, 4});

  ASSERT_TRUE(mln::ranges::equal(z, ref));
}


TEST(Range, transform_reversible_range)
{
  mln::ranges::mdindex<2>         ids({4, 3});
  std::vector<std::array<int, 2>> ref = {{9, 4}, {4, 4}, {1, 4}, {0, 4}, //
                                         {9, 1}, {4, 1}, {1, 1}, {0, 1}, //
                                         {9, 0}, {4, 0}, {1, 0}, {0, 0}};

  auto pow2 = mln::ranges::view::transform(ids, [](auto vals) {
    return std::array<int, 2>{vals[0] * vals[0], vals[1] * vals[1]};
  });
  auto res  = mln::ranges::view::reverse(pow2);

  static_assert(mln::ranges::MDBidirectionalRange<decltype(ids)>);
  static_assert(mln::ranges::MDBidirectionalRange<decltype(res)>);

  std::vector<std::array<int, 2>> out;
  for (auto&& r : res.rows())
    ::ranges::copy(r, ::ranges::back_inserter(out));

  ASSERT_EQ(ref, out);
}
