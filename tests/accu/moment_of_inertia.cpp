#include <mln/accu/accumulators/moment_of_inertia.hpp>
#include <mln/core/vec.hpp>

#include <gtest/gtest.h>

// Moment of inertia of a square
TEST(Accu, Moment_of_inertia)
{
  using V = mln::vec2i;

  auto sum_of_square = [](float x) { return x * (x + 1) * (2 * x + 1) / 6; };

  double res1;
  {
    mln::accu::accumulators::moment_of_inertia<V, V> acc;
    acc.init();
    int n = 10;
    for (short i = 0; i <= n; ++i)
      for (short j = 0; j <= n; ++j)
        acc.take(V{i, j});
    res1 = acc.to_result();

    double tmp = 2 * (n + 1) * sum_of_square(n / 2.f);
    double ref = (tmp + tmp) / mln::sqr(mln::sqr(n + 1));
    ASSERT_EQ(res1, ref);
  }

  // We are supposed to be scale invariant.
  {
    mln::accu::accumulators::moment_of_inertia<V, mln::vec<double, 2>> acc;
    acc.init();
    int n = 10000;
    for (short i = 0; i <= n; ++i)
      for (short j = 0; j <= n; ++j)
        acc.take(V{i, j});

    double res2 = acc.to_result();
    double tmp  = 2 * (n + 1) * sum_of_square(n / 2.f);
    double ref  = (tmp + tmp) / mln::sqr(mln::sqr(double(n + 1)));
    ASSERT_TRUE(std::abs(res2 - ref) <= 1);
    ASSERT_TRUE(std::abs(res1 - res2) <= 1);
  }
}
