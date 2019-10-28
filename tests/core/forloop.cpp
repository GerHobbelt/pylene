#include <mln/core/image/image.hpp>
#include <mln/core/domain/box.hpp>

#include <gtest/gtest.h>

TEST(Core, foreach_break)
{
  mln::box2d   box = {{-1, -1}, {5, 5}};
  mln::point2d p   = {0, 0};
  mln_foreach (p, box)
    if (p[0] == 2)
      break;

  ASSERT_TRUE((p == mln::point2d{2, -1}));
}

TEST(Core, foreach_0)
{
  mln::box2d   box = {{-1, -1}, {5, 5}};
  mln::point2d p   = {0, 0};

  int y = -1;
  int x = -1;
  mln_foreach (p, box)
  {
    ASSERT_EQ(p[0], y);
    ASSERT_EQ(p[1], x);
    if (++x == 5)
    {
      y++;
      x = -1;
    }
  }

  ASSERT_EQ(p, (mln::point2d{4, 4}));
}

TEST(Core, forall_break)
{
  mln::box2d box = {{-1, -1}, {5, 5}};
  auto       p   = mln::rng::iter(box);
  mln_forall (p)
    if ((*p)[0] == 2)
      break;

  ASSERT_TRUE((*p == mln::point2d{2, -1}));
}

TEST(Core, forall_0)
{
  mln::box2d box = {{-1, -1}, {5, 5}};
  auto       p   = mln::rng::iter(box);
  mln_forall (p)
  {
  }

  ASSERT_TRUE((*p > mln::point2d{4, 4}));
}
