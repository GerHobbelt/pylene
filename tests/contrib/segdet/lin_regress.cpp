//
// Created by alexandre on 28/05/2021.
//
#include <mln/contrib/segdet/linearregression.hpp>
#include <gtest/gtest.h>

TEST(Segdet, lin_regress_basic_0)
{
  using namespace mln::contrib::segdet;

  Linear_Regression<double, uint32_t> reg{};

  auto X = std::vector<uint32_t>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
  auto Y = std::vector<uint32_t>({2, 4, 6, 8, 10, 12, 14, 16, 18, 20});

  std::vector<std::vector<uint32_t>> Z({X, Y});

  reg.fit(Z);
  auto slope = reg.b_1;

  ASSERT_EQ(slope, 2);
}

TEST(Segdet, lin_regress_basic_1)
{
  using namespace mln::contrib::segdet;

  Linear_Regression<double, uint32_t> reg{};

  auto X = std::vector<uint32_t>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
  auto Y = std::vector<uint32_t>({2, 4, 6, 8, 10, 1, 14, 16, 18, 20});

  std::vector<std::vector<uint32_t>> Z({X, Y});

  reg.fit(Z);
  auto slope = reg.b_1;

  ASSERT_NE(slope, 2);
}