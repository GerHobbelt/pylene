#include <mln/core/algorithm/iota.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/morphers/extended_by_value_image.hpp>
#include <mln/core/neighb2d.hpp>

#include <gtest/gtest.h>

TEST(Core, value_extension_lvalue)
{
  using namespace mln;

  box2d        dom{{-1, -2}, {3, 3}};
  image2d<int> ima(dom);

  iota(ima, 0);
  auto x = extend_by_value(ima, 69);

  {
    mln_pixter(p, q, ima, x);
    mln_forall (p, q)
      ASSERT_EQ(p->val(), q->val());
  }

  {
    mln_pixter(p, x);
    mln_iter(q, c8(p));
    mln_forall (p)
      mln_forall (q)
      {
        if (!x.domain().has(q->point()))
          ASSERT_EQ(q->val(), 69);
        else
          ASSERT_EQ(&q->val(), &ima(q->point()));
      }
  }
}

TEST(Core, value_extension_rvalue)
{
  using namespace mln;

  box2d        dom{{-1, -2}, {3, 3}};
  image2d<int> ima(dom);

  iota(ima, 0);
  auto x = extend_by_value(ima, 69);

  {
    mln_pixter(p, q, ima, x);
    mln_forall (p, q)
      ASSERT_EQ(p->val(), q->val());
  }

  {
    mln_pixter(p, x);
    mln_iter(q, c8(p));
    mln_forall (p)
      mln_forall (q)
      {
        if (!x.domain().has(q->point()))
          ASSERT_EQ(q->val(), 69);
        else
          ASSERT_EQ(&q->val(), &ima(q->point()));
      }
  }
}
