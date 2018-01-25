#include <mln/core/se/rect2d.hpp>
#include <gtest/gtest.h>

TEST(Core, Rect2d)
{
  mln::se::rect2d win(5, 3);

  mln_iter(p, win(mln::literal::zero));
  p.init();
  for (int i = -1; i <= 1; ++i)
    for (int j = -2; j <= 2; ++j, p.next())
      ASSERT_EQ(*p, mln::point2d(i, j));
}
