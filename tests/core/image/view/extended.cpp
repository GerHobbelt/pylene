#include <mln/core/algorithm/iota.hpp>
#include <mln/core/box.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/image/view/extended.hpp>
#include <mln/core/image/view/zip.hpp>
#include <mln/core/neighborhood/c8.hpp>
#include <mln/core/rangev3/foreach.hpp>

#include <gtest/gtest.h>


TEST(View, extended)
{
  mln::experimental::box2d        dom{{-1, -2}, {3, 3}};
  mln::experimental::image2d<int> ima(dom);

  mln::iota(ima, 0);
  auto u = mln::view::value_extended(ima, 69);
  auto x = mln::view::extended(ima, u);

  {
    auto zz = mln::view::zip(ima, x);
    mln_foreach_new (auto px, zz.new_pixels())
    {
      auto [v1, v2] = px.val();
      ASSERT_EQ(v1, v2);
    }
  }

  {
    mln_foreach_new (auto p, x.new_pixels())
      for (auto q : mln::experimental::c8(p))
        if (!x.domain().has(q.point()))
          ASSERT_EQ(q.val(), 69);
        else
          ASSERT_EQ(&q.val(), &ima(q.point()));
  }
}
