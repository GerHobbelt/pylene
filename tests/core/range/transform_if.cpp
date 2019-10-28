#include <mln/core/rangev3/view/transform_if.hpp>

#include <mln/core/rangev3/algorithm/for_each.hpp>
#include <mln/core/box.hpp>
#include <mln/core/rangev3/mdspan.hpp>


#include <vector>
#include <gtest/gtest.h>

TEST(Core, transform_if_read)
{
  std::vector<int> data = {0, 1, 2, 3, 4, 5, 6, 7, 8};
  std::vector<int> ref = {0, 2, 4, 6, 8};


  // Chessboard only
  auto box = mln::experimental::box2d(3, 3);
  auto sp = mln::ranges::mdspan<int, 2>(data.data(), {3, 3});

  auto pred = [](mln::experimental::point2d p, int) { return (p.x() % 2) == (p.y() % 2); };
  auto map  = [](mln::experimental::point2d, int v){ return v; };
  auto rng = mln::ranges::view::transform_if(map, pred, box, sp);

  static_assert(mln::ranges::MDRange<decltype(rng)>);

  std::vector<int> res;
  mln::ranges::for_each(rng, [&res](int v) { res.push_back(v); });

  ASSERT_EQ(res, ref);
}


TEST(Core, transform_if_write)
{
  std::vector<int> data = {0, 1, 2, 3, 4, 5, 6, 7, 8};
  std::vector<int> ref  = {42, 1, 42, 3, 42, 5, 42, 7, 42};


  // Chessboard only
  auto box = mln::experimental::box2d(3, 3);
  auto sp = mln::ranges::mdspan<int, 2>(data.data(), {3, 3});

  auto pred = [](mln::experimental::point2d p, int) { return (p.x() % 2) == (p.y() % 2); };
  auto map  = [](mln::experimental::point2d, int& v) -> int& { return v; };
  auto rng = mln::ranges::view::transform_if(map, pred, box, sp);

  static_assert(mln::ranges::MDRange<decltype(rng)>);

  std::vector<int> res;
  mln::ranges::for_each(rng, [](int& v) { v = 42; });

  ASSERT_EQ(data, ref);
}

