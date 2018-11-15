#include <mln/core/image/view/transform.hpp>
#include <mln/core/image/image2d.hpp>

#include <mln/core/rangev3/view/zip.hpp>

#include <mln/core/algorithm/iota.hpp>
#include <mln/core/algorithm/fill.hpp>

#include <mln/core/rangev3/foreach.hpp>

#include <gtest/gtest.h>
#include <helpers.hpp>

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

TEST(Core, TransformedImage_transform_byval_chain)
{
  using namespace mln;

  box2d dom{{-1, -2}, {3, 3}};
  image2d<int> ima(dom);

  iota(ima, 0);
  {
    auto out = view::transform(view::transform(ima, [](int x) { return x * 2; }), [](int x) { return x * 2; });

    // Test pixel iteration
    // check that properties of pixels are preserved (point + index)
    {
      mln_foreach_new((auto [px1, px2]), mln::ranges::view::zip(ima.new_pixels(), out.new_pixels()))
      {
        ASSERT_EQ(px1.point(), px2.point());
        //ASSERT_EQ(px1.index(), px2.index());
        ASSERT_EQ(4 * px1.val(), px2.val());
        //ASSERT_EQ(&px2->image(), &out);
      }
    }

    // Test value iteration
    {
      mln_foreach_new((auto [v1, v2]), mln::ranges::view::zip(ima.new_values(), out.new_values()))
        ASSERT_EQ(4 * v1, v2);
    }
  }
}

TEST(Core, Transform_Support_Writable)
{
  using namespace mln;

  typedef std::pair<int, int> V;

  box2d dom{{-1, -2}, {3, 3}};
  image2d<V> ima(dom);
  {
    auto c1 = view::transform(ima, [](std::pair<int, int>& x) -> int& { return x.first; });
    auto c2 = view::transform(ima, [](const std::pair<int, int>& x) { return x.second; });
    fill(ima, std::make_pair(12, 12));

    // FIXME: use fill once adapted
    mln_foreach_new(auto&& val, c1.new_values())
      val = 69;

    mln_foreach_new(auto&& val, c2.new_values()) // Compile but writing a temporary (no effect)
      val = 69;

    // Test pixel iteration
    // check that properties of pixels are preserved (point + index)
    {
      mln_foreach_new((auto [px1, px2, px3]), mln::ranges::view::zip(ima.new_pixels(), c1.new_pixels(), c2.new_pixels()))
      {
        ASSERT_EQ(px1.point(), px2.point());
        ASSERT_EQ(px1.point(), px3.point());
        //ASSERT_EQ(px1.index(), px2.index());
        ASSERT_EQ(std::make_pair(69, 12), px1.val());
        ASSERT_EQ(69, px2.val());
        ASSERT_EQ(12, px3.val());
        //ASSERT_EQ(&px2->image(), &out);
      }
    }

    // Test value iteration
    {
      mln_foreach_new((auto [v1, v2, v3]), mln::ranges::view::zip(ima.new_values(), c1.new_values(), c2.new_values()))
      {
        ASSERT_EQ(std::make_pair(69, 12), v1);
        ASSERT_EQ(69, v2);
        ASSERT_EQ(12, v3);
      }
    }
  }
}

int times_two(int x)
{
  return x * 2;
}



TEST(Core, Transform_Supports_Function)
{
  mln::box2d dom{{-1, -2}, {3, 3}};
  mln::image2d<int> ima(dom, 3, 1);
  mln::image2d<int> ref(dom, 3, 2);

  auto c = mln::view::transform(ima, times_two);

  // FIXME: ASSERT_IMAGES_EQ(ref, c);
}

TEST(Core, Transform_Supports_PointerToMemberFunction)
{
  typedef std::pair<int, int> V;

  mln::box2d dom{{-1, -2}, {3, 3}};
  mln::image2d<V> ima(dom, 3, std::make_pair(42,42));
  mln::image2d<V> ref(dom, 3, std::make_pair(69,42));

  auto c = mln::view::transform(ima, &std::pair<int,int>::first);

  // FIXME: mln::fill(c, 69);

  mln_foreach_new(auto& val, c.new_values())
    val = 69;


  // FIXME: ASSERT_IMAGES_EQ(ref, c);
}


