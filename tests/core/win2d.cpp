#include <mln/core/win2d.hpp>

#include <gtest/gtest.h>

TEST(Core, win2d_general)
{
  using namespace mln;
  {
    rect2d win = make_rectangle2d(3, 5);

    mln_iter(p, win(literal::zero));
    p.init();
    for (int i = -1; i <= 1; ++i)
      for (int j = -2; j <= 2; ++j, p.next())
        ASSERT_EQ(*p, point2d(i, j));
  }
}
