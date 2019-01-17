#include <mln/core/image/image2d.hpp>
#include <mln/core/image/private/image_operators.hpp>
#include <mln/core/image/view/filter.hpp>

#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>

#include <mln/core/rangev3/foreach.hpp>
#include <mln/core/rangev3/view/zip.hpp>

#include <helpers.hpp>

#include <gtest/gtest.h>


TEST(View, filter_readonly)
{
  using namespace mln;
  using namespace mln::experimental::ops;

  box2d        dom{{-1, -2}, {3, 3}};
  image2d<int> ima(dom);

  iota(ima, 0);
  auto x = view::filter(ima, [](int v) { return v > 10; });

  ASSERT_TRUE(mln::experimental::all(x > 10));

  mln_foreach_new (auto&& pix, ima.new_pixels())
  {
    (void)pix;
    // FIXME:
    // if (pix.val() > 10)
    //  ASSERT_EQ(pix.val(), x(pix.point()));
    // else
    //  ASSERT_EQ(pix.val(), x.at(pix.point()));
  }

  mln_foreach_new (auto&& pix, x.new_pixels())
    ASSERT_EQ(pix.val(), ima(pix.point()));
}


TEST(View, filter_writable)
{
  using namespace mln;
  using namespace mln::experimental::ops;

  box2d        dom{{-1, -2}, {3, 3}};
  image2d<int> ima(dom);


  iota(ima, 0);
  auto x = view::filter(ima, [](int v) { return v > 10; });

  fill(x, 10);
  ASSERT_TRUE(mln::experimental::all(ima <= 10));
}


TEST(View, filter_twice)
{
  using namespace mln;
  using namespace mln::experimental::ops;

  box2d        dom{{-1, -2}, {3, 3}};
  image2d<int> ima(dom);

  iota(ima, 0);
  auto x = view::filter(ima, [](int v) { return v > 10; });
  auto u = view::filter(x, [](int v) { return v < 15; });

  ASSERT_TRUE(mln::experimental::all(land(u > 10, u < 15)));

  mln_foreach_new (auto&& pix, ima.new_pixels())
  {
    (void)pix;
    // FIXME:
    // if (pix.val() > 10 and pix.val() < 15)
    //   ASSERT_EQ(pix.val(), u(pix.point()));
    // else
    //   ASSERT_EQ(pix.val(), u.at(pix.point()));
    // ASSERT_TRUE(!u.domain().has(pix.point()));
  }


  {
    mln_foreach_new (auto&& pix, u.new_pixels())
      ASSERT_EQ(pix.val(), ima(pix.point()));
  }

  image2d<int> before = clone(ima);
  fill(u, 1);
  {
    mln_foreach_new ((auto [old_v, new_v]), mln::ranges::view::zip(before.new_values(), u.new_values()))
    {
      (void)old_v;
      (void)new_v;
      // FIXME:
      // if (old_v > 10 && old_v < 15)
      //   ASSERT_EQ(1, new_v);
      // else
      //   ASSERT_EQ(old_v, new_v);
    }
  }

  mln_foreach_new (auto&& px, u.new_pixels())
    ASSERT_EQ(px.val(), ima(px.point()));
}
