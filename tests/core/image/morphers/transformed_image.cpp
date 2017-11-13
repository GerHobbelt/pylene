#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/morphers/transformed_image.hpp>
#include <mln/core/neighb2d.hpp>

#include <gtest/gtest.h>

TEST(Core, TransformedImage_transform_byval_rvalue)
{
  using namespace mln;

  box2d dom{{-1, -2}, {3, 3}};
  image2d<int> ima(dom);

  iota(ima, 0);
  {
    auto out = imtransform(ima, [](int x) { return x * 2; });

    // Test pixel iteration
    // check that properties of pixels are preserved (point + index)
    {
      mln_pixter(px1, ima);
      mln_pixter(px2, out);
      mln_forall (px1, px2)
      {
        ASSERT_EQ(px1->point(), px2->point());
        ASSERT_EQ(px1->index(), px2->index());
        ASSERT_EQ(2 * px1->val(), px2->val());
        ASSERT_EQ(&px2->image(), &out);
      }
    }

    // Test value iteration
    {
      mln_viter(v1, ima);
      mln_viter(v2, out);
      mln_forall (v1, v2)
        ASSERT_EQ(2 * *v1, *v2);
    }
  }
}

TEST(Core, TransformedImage_transform_byval_chain)
{
  using namespace mln;

  box2d dom{{-1, -2}, {3, 3}};
  image2d<int> ima(dom);

  iota(ima, 0);
  {
    auto out = imtransform(imtransform(ima, [](int x) { return x * 2; }), [](int x) { return x * 2; });

    // Test pixel iteration
    // check that properties of pixels are preserved (point + index)
    {
      mln_pixter(px1, ima);
      mln_pixter(px2, out);
      mln_forall (px1, px2)
      {
        ASSERT_EQ(px1->point(), px2->point());
        ASSERT_EQ(px1->index(), px2->index());
        ASSERT_EQ(4 * px1->val(), px2->val());
        ASSERT_EQ(&px2->image(), &out);
      }
    }

    // Test value iteration
    {
      mln_viter(v1, ima);
      mln_viter(v2, out);
      mln_forall (v1, v2)
        ASSERT_EQ(4 * *v1, *v2);
    }
  }
}

TEST(Core, TransformedImage_transform_byval_lvalue)
{
  using namespace mln;

  typedef std::pair<int, int> V;

  box2d dom{{-1, -2}, {3, 3}};
  image2d<V> ima(dom);

  {
    auto c1 = imtransform(ima, [](std::pair<int, int>& x) -> int& { return x.first; });
    auto c2 = imtransform(ima, [](const std::pair<int, int>& x) -> const int& { return x.second; });
    fill(ima, std::make_pair(12, 12));
    fill(c1, 69);

    // Test pixel iteration
    // check that properties of pixels are preserved (point + index)
    {
      mln_pixter(px0, ima);
      mln_pixter(px1, c1);
      mln_pixter(px2, c2);
      mln_forall (px0, px1, px2)
      {
        ASSERT_EQ(px1->point(), px0->point());
        ASSERT_EQ(px1->index(), px0->index());
        ASSERT_EQ(px1->val(), px0->val().first);
        ASSERT_EQ(px1->val(), 69);
        ASSERT_EQ(px2->val(), 12);
      }
    }

    // Test value iteration
    {
      mln_viter(v0, ima);
      mln_viter(v1, c1);
      mln_viter(v2, c2);
      mln_forall (v0, v1, v2)
      {
        ASSERT_EQ(v0->first, *v1);
        ASSERT_EQ(v0->second, *v2);
      }
    }
  }
}
