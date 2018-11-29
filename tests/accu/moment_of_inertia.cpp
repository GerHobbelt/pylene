#include <mln/accu/accumulators/moment_of_inertia.hpp>
#include <mln/core/point.hpp>

#include <gtest/gtest.h>

// Moment of inertia of a square
TEST(Accu, Moment_of_inertia)
{
  using namespace mln;

  auto sum_of_square = [](float x) { return x * (x + 1) * (2 * x + 1) / 6; };

  double res1;
  {
    accu::accumulators::moment_of_inertia<point2d, vec2u> acc;
    acc.init();
    int n = 10;
    for (short i = 0; i <= n; ++i)
      for (short j = 0; j <= n; ++j)
        acc.take(point2d{i, j});
    res1 = acc.to_result();

    double tmp = 2 * (n + 1) * sum_of_square(n / 2.f);
    double ref = (tmp + tmp) / sqr(sqr(n + 1));
    ASSERT_EQ(res1, ref);
  }

  // We are supposed to be scale invariant.
  {
    accu::accumulators::moment_of_inertia<point2d, vec<double, 2>> acc;
    acc.init();
    int n = 10000;
    for (short i = 0; i <= n; ++i)
      for (short j = 0; j <= n; ++j)
        acc.take(point2d{i, j});

    double res2 = acc.to_result();
    double tmp  = 2 * (n + 1) * sum_of_square(n / 2.f);
    double ref  = (tmp + tmp) / sqr(sqr(double(n + 1)));
    ASSERT_TRUE(std::abs(res2 - ref) <= 1);
    ASSERT_TRUE(std::abs(res1 - res2) <= 1);
  }
}
