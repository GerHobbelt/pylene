#include <mln/core/image/view/transform.hpp>

#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/concepts/archetype/image.hpp>
#include <mln/core/concepts/archetype/pixel.hpp>
#include <mln/core/concepts/pixel.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/adaptor.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/range/foreach.hpp>
#include <mln/core/range/view/zip.hpp>

#include <range/v3/algorithm/copy.hpp>
#include <range/v3/iterator/insert_iterators.hpp>
#include <vector>

#include <fixtures/ImageCompare/image_compare.hpp>
#include <gtest/gtest.h>

TEST(Core, Transform_Supports_RValue_Lambda)
{
  mln::box2d        dom({-2, -1}, {3, 3});
  mln::image2d<int> ima(dom);

  mln::iota(ima, 0);
  {
    auto out = mln::view::transform(ima, [](int x) { return x * 2; });


    static_assert(mln::concepts::ConcreteImage<decltype(ima)>);
    static_assert(mln::concepts::OutputImage<decltype(ima)>);
    static_assert(mln::concepts::ViewImage<decltype(out)>);
    static_assert(mln::concepts::IndexableAndAccessibleImage<decltype(out)>);
    static_assert(mln::concepts::BidirectionalImage<decltype(out)>);
    static_assert(not mln::concepts::RawImage<decltype(out)>);
    static_assert(not mln::concepts::OutputImage<decltype(out)>);

    // Test pixel iteration
    // check that properties of pixels are preserved (point + index)
    {
      auto rng = mln::ranges::view::zip(ima.pixels(), out.pixels());
      mln_foreach ((auto [px1, px2]), rng)
      {
        ASSERT_EQ(px1.point(), px2.point());
        // ASSERT_EQ(px1.index(), px2.index());
        ASSERT_EQ(2 * px1.val(), px2.val());
        // ASSERT_EQ(&px2->image(), &out);
      }
    }

    // Test value iteration
    {
      auto rng = mln::ranges::view::zip(ima.values(), out.values());
      mln_foreach ((auto [v1, v2]), rng)
        ASSERT_EQ(2 * v1, v2);
    }
  }
}


int plus_one(int x)
{
  return x + 1;
}


TEST(Core, Transform_Supports_Function)
{
  mln::box2d dom({-2, -1}, {3, 3});

  mln::image2d<int> ima(dom);
  mln::image2d<int> ref(dom);
  mln::iota(ima, 0);
  mln::iota(ref, 1);

  auto c = mln::view::transform(ima, plus_one);
  static_assert(mln::concepts::ConcreteImage<decltype(ima)>);
  static_assert(mln::concepts::OutputImage<decltype(ima)>);
  static_assert(mln::concepts::ViewImage<decltype(c)>);
  static_assert(mln::concepts::IndexableAndAccessibleImage<decltype(c)>);
  static_assert(mln::concepts::BidirectionalImage<decltype(c)>);
  static_assert(not mln::concepts::RawImage<decltype(c)>);
  static_assert(not mln::concepts::OutputImage<decltype(c)>);

  ASSERT_IMAGES_EQ_EXP(ref, c);
}

TEST(Core, Transform_Supports_PointerToMemberFunction)
{
  using V = std::pair<int, int>;

  mln::box2d              dom({-2, -1}, {3, 3});
  mln::image_build_params params;
  params.border = 3;

  params.init_value = std::make_pair(42, 42);
  mln::image2d<V> ima(dom, params);

  params.init_value = std::make_pair(69, 42);
  mln::image2d<V> ref(dom, params);

  auto c = mln::view::transform(ima, &V::first);

  static_assert(mln::concepts::ConcreteImage<decltype(ima)>);
  static_assert(mln::concepts::OutputImage<decltype(ima)>);
  static_assert(mln::concepts::ViewImage<decltype(c)>);
  static_assert(mln::concepts::IndexableAndAccessibleImage<decltype(c)>);
  static_assert(mln::concepts::BidirectionalImage<decltype(c)>);
  static_assert(not mln::concepts::RawImage<decltype(c)>);
  static_assert(mln::concepts::OutputImage<decltype(c)>);

  mln::fill(c, 69);
  ASSERT_IMAGES_EQ_EXP(ima, ref);
}

TEST(Core, Transform_Twice)
{
  mln::box2d dom({-2, -1}, {3, 3});

  mln::image2d<int> ima(dom);
  mln::image2d<int> ref(dom);

  mln::iota(ima, 0);
  mln::iota(ref, 2);

  auto out = mln::view::transform(mln::view::transform(ima, plus_one), plus_one);


  static_assert(mln::concepts::ConcreteImage<decltype(ima)>);
  static_assert(mln::concepts::OutputImage<decltype(ima)>);
  static_assert(mln::concepts::ViewImage<decltype(out)>);
  static_assert(mln::concepts::IndexableAndAccessibleImage<decltype(out)>);
  static_assert(mln::concepts::BidirectionalImage<decltype(out)>);
  static_assert(not mln::concepts::RawImage<decltype(out)>);
  static_assert(not mln::concepts::OutputImage<decltype(out)>);

  ASSERT_IMAGES_EQ_EXP(out, ref);
}

TEST(Core, Transform_Support_Writable)
{
  using V = std::pair<int, int>;

  mln::box2d      dom({-2, -1}, {3, 3});
  mln::image2d<V> ima(dom);

  auto c1 = mln::view::transform(ima, [](std::pair<int, int>& x) -> int& { return x.first; });
  auto c2 = mln::view::transform(ima, [](const std::pair<int, int>& x) { return x.second; });
  mln::fill(ima, std::make_pair(12, 12));


  static_assert(mln::concepts::ViewImage<decltype(c1)>);
  static_assert(mln::concepts::IndexableAndAccessibleImage<decltype(c1)>);
  static_assert(mln::concepts::BidirectionalImage<decltype(c1)>);
  static_assert(not mln::concepts::RawImage<decltype(c1)>);
  static_assert(mln::concepts::OutputImage<decltype(c1)>);

  static_assert(mln::concepts::ViewImage<decltype(c2)>);
  static_assert(mln::concepts::IndexableAndAccessibleImage<decltype(c2)>);
  static_assert(mln::concepts::BidirectionalImage<decltype(c2)>);
  static_assert(not mln::concepts::RawImage<decltype(c2)>);
  static_assert(not mln::concepts::OutputImage<decltype(c2)>);


  mln::fill(c1, 69);

  // Does not compile because direct writing a temporary is forbidden by range-v3
  // fill(c2, 69);

  // Test pixel iteration
  // check that properties of pixels are preserved (point + index)
  {
    auto rng = mln::ranges::view::zip(ima.pixels(), c1.pixels(), c2.pixels());
    mln_foreach ((auto [px1, px2, px3]), rng)
    {
      ASSERT_EQ(px1.point(), px2.point());
      ASSERT_EQ(px1.point(), px3.point());
      // ASSERT_EQ(px1.index(), px2.index());
      ASSERT_EQ(std::make_pair(69, 12), px1.val());
      ASSERT_EQ(69, px2.val());
      ASSERT_EQ(12, px3.val());
      // ASSERT_EQ(&px2->image(), &out);
    }
  }

  // Test value iteration
  {
    auto rng = mln::ranges::view::zip(ima.values(), c1.values(), c2.values());
    mln_foreach ((auto [v1, v2, v3]), rng)
    {
      ASSERT_EQ(std::make_pair(69, 12), v1);
      ASSERT_EQ(69, v2);
      ASSERT_EQ(12, v3);
    }
  }
}


TEST(Core, Transformed2Image_transform_byval_chain)
{
  mln::box2d        dom({-2, -1}, {3, 3});
  mln::image2d<int> ima(dom);
  mln::image2d<int> ima2(dom);

  mln::iota(ima, 0);
  mln::iota(ima2, 1);

  auto x   = mln::view::transform(ima, [](int a) { return a > 3; });
  auto y   = mln::view::transform(x, ima, [](bool a, int b) { return a ? b : 2; });
  auto out = mln::view::transform(y, ima2, [](int a, int b) { return a + b; });

  std::vector<int> ref = {3, 4, 5, 6, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39};

  std::vector<int> tmp;
  auto&&           vals = out.values();
  for (auto&& r : mln::ranges::rows(vals))
    ::ranges::copy(r, ::ranges::back_inserter(tmp));

  ASSERT_EQ(ref, tmp);
}
