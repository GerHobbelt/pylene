#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/private/image_operators.hpp>
#include <mln/core/image/view/filter.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/io/imprint.hpp>

#include <gtest/gtest.h>


TEST(Core, FilteredImage_filtered_byval)
{
  using namespace mln;
  using namespace mln::experimental::ops;

  box2d        dom{{-1, -2}, {3, 3}};
  image2d<int> ima(dom);

  iota(ima, 0);
  auto x = view::filter(ima, [](int v) { return v > 10; });

  ASSERT_TRUE(all_of(x > 10));

  {
    for (auto&& pix : ima.new_pixels())
    {
      if (pix.val() > 10)
        ASSERT_EQ(pix.val(), x(pix.point()));
      else
      {
        ASSERT_TRUE(!x.domain().has(pix.point()));
        ASSERT_EQ(pix.val(), x.at(pix.point()));
      }
    }
  }

  {
    for (auto&& pix : x.new_pixels())
    {
      ASSERT_EQ(pix.val(), ima(pix.point()));
    }
  }
}

TEST(Core, FilteredImage_filtered_bypix_writing)
{
  using namespace mln;
  using namespace mln::experimental::ops;

  box2d        dom{{-1, -2}, {3, 3}};
  image2d<int> ima(dom);

  iota(ima, 0);
  auto x = view::filter(ima, [](int v) { return v > 10; });

  mln::fill(x, 10);
  ASSERT_TRUE(all_of(ima <= 10));
}

TEST(Core, FilteredImage_filtered_byval_writing)
{
  using namespace mln;
  using namespace mln::experimental::ops;

  box2d        dom{{-1, -2}, {3, 3}};
  image2d<int> ima(dom);

  iota(ima, 0);
  auto x = view::filter(ima, [](int x) { return x > 10; });

  mln::fill(x, 10);
  ASSERT_TRUE(all_of(ima <= 10));
}

TEST(Core, FilteredImage_filtered_chaining)
{
  using namespace mln;
  using namespace mln::experimental::ops;

  box2d        dom{{-1, -2}, {3, 3}};
  image2d<int> ima(dom);

  iota(ima, 0);
  auto x = view::filter(ima, [](int v) { return v > 10; });
  auto u = view::filter(x, [](int v) { return v < 15; });

  ASSERT_TRUE(all_of(u > 10 && u < 15));

  {
    for (auto&& pix : ima.new_pixels())
      if (pix.val() > 10 and pix.val() < 15)
        ASSERT_EQ(pix.val(), u(pix.point()));
      else
      {
        ASSERT_TRUE(!u.domain().has(pix.point()));
        ASSERT_EQ(pix.val(), u.at(pix.point()));
      }
  }

  {
    for (auto&& pix : u.new_pixels())
    {
      ASSERT_EQ(pix.val(), ima(pix.point()));
    }
  }

  const image2d<int> before = clone(ima);
  mln::fill(u, 1);
  {
    auto z = view::zip(ima, before);
    for (auto&& z_pix : z.new_pixels())
    {
      auto&& [v_ima, v_before] = z_pix.val();
      if (v_before > 10 and v_before < 15)
        ASSERT_EQ(v_ima, 1);
      else
      {
        ASSERT_TRUE(!u.domain().has(z_pix.point()));
        ASSERT_EQ(v_ima, v_before);
      }
    }
  }

  {
    for (auto&& pix : u.new_pixels())
    {
      ASSERT_EQ(pix.val(), ima(pix.point()));
    }
  }
}
