#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/io/imprint.hpp>

#include <gtest/gtest.h>

TEST(Core, FilteredImage_filtered_bypix)
{
  using namespace mln;

  typedef image2d<int> I;

  box2d        dom{{-1, -2}, {3, 3}};
  image2d<int> ima(dom);

  typedef I::const_pixel_type Pix;

  iota(ima, 0);
  auto x = imfilter(ima, [](const Pix& px) { return px.val() > 10; });

  ASSERT_TRUE(all(x > 10));

  {mln_foreach (const Pix& pix, ima.pixels()){if (pix.val() > 10) ASSERT_EQ(pix.val(), x(pix.point()));
  else
  {
    ASSERT_TRUE(!x.domain().has(pix.point()));
    ASSERT_EQ(pix.val(), x.at(pix.point()));
  }
}
}

{
  mln_foreach (const auto& pix, x.pixels())
  {
    ASSERT_EQ(pix.val(), ima(pix.point()));
  }
}
}

TEST(Core, FilteredImage_filtered_byval)
{
  using namespace mln;

  typedef image2d<int> I;

  box2d        dom{{-1, -2}, {3, 3}};
  image2d<int> ima(dom);

  typedef I::const_pixel_type Pix;

  iota(ima, 0);
  auto x = imfilter(ima, [](int v) { return v > 10; });

  ASSERT_TRUE(all(x > 10));

  {
    mln_foreach (const Pix& pix, ima.pixels())
      if (pix.val() > 10)
        ASSERT_EQ(pix.val(), x(pix.point()));
      else
      {
        ASSERT_TRUE(!x.domain().has(pix.point()));
        ASSERT_EQ(pix.val(), x.at(pix.point()));
      }
  }

  {
    mln_foreach (const auto& pix, x.pixels())
    {
      ASSERT_EQ(pix.val(), ima(pix.point()));
    }
  }
}

TEST(Core, FilteredImage_filtered_bypix_writing)
{
  using namespace mln;

  typedef image2d<int> I;

  box2d        dom{{-1, -2}, {3, 3}};
  image2d<int> ima(dom);

  typedef I::const_pixel_type Pix;

  iota(ima, 0);
  auto x = imfilter(ima, [](const Pix& px) { return px.val() > 10; });

  mln::fill(x, 10);
  ASSERT_TRUE(all(ima <= 10));
}

TEST(Core, FilteredImage_filtered_byval_writing)
{
  using namespace mln;

  box2d        dom{{-1, -2}, {3, 3}};
  image2d<int> ima(dom);

  iota(ima, 0);
  auto x = imfilter(ima, [](int x) { return x > 10; });

  mln::fill(x, 10);
  ASSERT_TRUE(all(ima <= 10));
}

TEST(Core, FilteredImage_filtered_chaining)
{
  using namespace mln;

  box2d        dom{{-1, -2}, {3, 3}};
  image2d<int> ima(dom);

  iota(ima, 0);
  auto x = imfilter(ima, [](int v) { return v > 10; });
  auto u = imfilter(x, [](int v) { return v < 15; });

  ASSERT_TRUE(all(land(u > 10, u < 15)));

  {
    mln_foreach (const auto& pix, ima.pixels())
      if (pix.val() > 10 and pix.val() < 15)
        ASSERT_EQ(pix.val(), u(pix.point()));
      else
      {
        ASSERT_TRUE(!u.domain().has(pix.point()));
        ASSERT_EQ(pix.val(), u.at(pix.point()));
      }
  }

  {
    mln_foreach (const auto& pix, u.pixels())
    {
      ASSERT_EQ(pix.val(), ima(pix.point()));
    }
  }

  const image2d<int> before = clone(ima);
  mln::fill(u, 1);
  {
    mln_pixter(px, ima);
    mln_pixter(px_before, before);
    mln_forall (px, px_before)
    {
      if (px_before->val() > 10 and px_before->val() < 15)
        ASSERT_EQ(px->val(), 1);
      else
      {
        ASSERT_TRUE(!u.domain().has(px->point()));
        ASSERT_EQ(px->val(), px_before->val());
      }
    }
  }
  {
    mln_foreach (const auto& pix, u.pixels())
    {
      ASSERT_EQ(pix.val(), ima(pix.point()));
    }
  }
}
