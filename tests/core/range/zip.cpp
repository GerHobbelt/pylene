#include <mln/core/image/image.hpp>
#include <mln/core/range/zip.hpp>
#include <mln/core/rangev3/multi_indices.hpp>
#include <mln/core/domain/box.hpp>
#include <mln/core/rangev3/view/zip.hpp>
#include <mln/core/rangev3/rows.hpp>

#include <array>
#include <iostream>

#include <gtest/gtest.h>

TEST(Core, ziprange)
{

  using namespace mln;

  std::array<int, 4> x{{2, 5, 15, 22}};
  std::array<int, 4> y{{-2, -5, -15, -22}};

  ASSERT_EQ(rng::size(x), 4u);

  auto z = rng::zip(x, y);

  {
    int i = 0;
    mln_foreach (auto v, z)
    {
      ASSERT_EQ(std::get<0>(v), x[i]);
      ASSERT_EQ(std::get<1>(v), y[i++]);
    }
  }

  {
    int i = 0;
    mln_iter(pv, z);
    mln_forall (pv)
    {
      ASSERT_EQ(std::get<0>(*pv), x[i]);
      ASSERT_EQ(std::get<1>(*pv), y[i++]);
    }
  }
}

TEST(Code, zip_multirow)
{

  mln::box2d multi_ind0 = {{0, 0}, {2, 3}};
  mln::box2d multi_ind1 = {{1, 2}, {3, 5}};
  mln::box2d multi_ind2 = {{2, 3}, {4, 6}};

  for (auto&& e : mln::ranges::view::zip(multi_ind0, multi_ind1, multi_ind2))
  {
    std::cout << std::get<0>(e) << " " << std::get<1>(e) << " " << std::get<2>(e) << '\n';
  }
}

TEST(Code, zip_multirow_rowwise)
{

  mln::box2d multi_ind0 = {{0, 0}, {2, 3}};
  mln::box2d multi_ind1 = {{1, 2}, {3, 5}};
  mln::box2d multi_ind2 = {{2, 3}, {4, 6}};

  const auto zipped_rows = mln::ranges::view::zip(multi_ind0, multi_ind1, multi_ind2);
  for (auto&& r : mln::ranges::rows(zipped_rows))
  {
    for(auto&& e : r)
    {
      std::cout << std::get<0>(e) << " " << std::get<1>(e) << " " << std::get<2>(e) << '\n';
    }
  }
}
