#include <mln/core/image/view/transform.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/rangev3/view/zip.hpp>
#include <mln/core/algorithm/iota.hpp>

#include <mln/core/rangev3/foreach.hpp>

#include <gtest/gtest.h>

TEST(Core, TransformedImage_transform_byval_rvalue)
{
  using namespace mln;

  box2d dom{{-1, -2}, {3, 3}};
  image2d<int> ima(dom);

  iota(ima, 0);
  {
    auto out = view::transform(ima, [](int x) { return x * 2; });

    // Test pixel iteration
    // check that properties of pixels are preserved (point + index)
    {
      mln_foreach_new((auto [px1, px2]), mln::ranges::view::zip(ima.new_pixels(), out.new_pixels()))
      {
        ASSERT_EQ(px1.point(), px2.point());
        //ASSERT_EQ(px1.index(), px2.index());
        ASSERT_EQ(2 * px1.val(), px2.val());
        //ASSERT_EQ(&px2->image(), &out);
      }
    }

    // Test value iteration
    {
      mln_foreach_new((auto [v1, v2]), mln::ranges::view::zip(ima.new_values(), out.new_values()))
        ASSERT_EQ(2 * v1, v2);
    }
  }
}
