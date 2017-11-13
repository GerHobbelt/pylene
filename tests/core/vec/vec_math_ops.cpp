#include <mln/core/vec.hpp>
#include <mln/core/vec/vec_math_ops.hpp>

#include <gtest/gtest.h>

TEST(Core, vec_math_ops)
{
  using namespace mln;

  {
    vec3b v = {-127, 2, -1};

    ASSERT_EQ(l0norm(v), 1);
    ASSERT_EQ(linfnorm(v), 127);
    ASSERT_EQ(l1norm(v), 130); // check type promotion
    ASSERT_EQ(l2norm(v), std::sqrt(127 * 127 + 4 + 1));
  }
}
