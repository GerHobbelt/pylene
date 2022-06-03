//
// Created by alexandre on 28/05/2021.
//
#include <gtest/gtest.h>

#include "../../pylene/src/scribo/segdet/linearregression.hpp"

TEST(Segdet, lin_regress_basic_0)
{
  using namespace scribo::internal;

  MemoryLinearRegressor reg{0, 0, 20};

  auto X = std::vector<uint32_t>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
  auto Y = std::vector<uint32_t>({2, 4, 6, 8, 10, 12, 14, 16, 18, 20});

  for (size_t i = 0; i < X.size(); i++)
  {
    reg.push(X[i], Y[i]);
  }

  auto slope = reg.predict();

  ASSERT_EQ(slope, 2);
}

TEST(Segdet, lin_regress_basic_1)
{
  using namespace scribo::internal;

  MemoryLinearRegressor reg{0, 0, 20};

  auto X = std::vector<uint32_t>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
  auto Y = std::vector<uint32_t>({2, 4, 6, 8, 10, 1, 14, 16, 18, 20});

  for (size_t i = 0; i < X.size(); i++)
  {
    reg.push(X[i], Y[i]);
  }

  auto slope = reg.predict();

  ASSERT_NE(slope, 2);
}