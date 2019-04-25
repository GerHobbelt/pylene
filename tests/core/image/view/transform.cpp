#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/equal.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/concept/new/archetype/image.hpp>
#include <mln/core/concept/new/archetype/pixel.hpp>
#include <mln/core/concept/new/pixels.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/view/adaptor.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/image/view/transform.hpp>
#include <mln/core/rangev3/foreach.hpp>
#include <mln/core/rangev3/view/zip.hpp>

#include <vector>

#include <gtest/gtest.h>


TEST(Core, TransformedImage_transform_byval_rvalue)
{
  using namespace mln;

  box2d        dom{{-1, -2}, {3, 3}};
  image2d<int> ima(dom);

  iota(ima, 0);
  {
    auto out = view::transform(ima, [](int x) { return x * 2; });

#ifdef PYLENE_CONCEPT_TS_ENABLED
    static_assert(mln::concepts::ConcreteImage<decltype(ima)>);
    static_assert(mln::concepts::OutputImage<decltype(ima)>);
    static_assert(mln::concepts::ViewImage<decltype(out)>);
    static_assert(mln::concepts::IndexableAndAccessibleImage<decltype(out)>);
    static_assert(mln::concepts::BidirectionalImage<decltype(out)>);
    static_assert(not mln::concepts::RawImage<decltype(out)>);
    static_assert(not mln::concepts::OutputImage<decltype(out)>);
#endif // PYLENE_CONCEPT_TS_ENABLED

    // Test pixel iteration
    // check that properties of pixels are preserved (point + index)
    {
      auto rng = mln::ranges::view::zip(ima.new_pixels(), out.new_pixels());
      mln_foreach_new ((auto [px1, px2]), rng)
      {
        ASSERT_EQ(px1.point(), px2.point());
        // ASSERT_EQ(px1.index(), px2.index());
        ASSERT_EQ(2 * px1.val(), px2.val());
        // ASSERT_EQ(&px2->image(), &out);
      }
    }

    // Test value iteration
    {
      auto rng = mln::ranges::view::zip(ima.new_values(), out.new_values());
      mln_foreach_new ((auto [v1, v2]), rng)
        ASSERT_EQ(2 * v1, v2);
    }
  }
}

TEST(Core, TransformedImage_transform_byval_chain)
{
  using namespace mln;

  box2d        dom{{-1, -2}, {3, 3}};
  image2d<int> ima(dom);

  iota(ima, 0);
  {
    auto out = view::transform(view::transform(ima, [](int x) { return x * 2; }), [](int x) { return x * 2; });

#ifdef PYLENE_CONCEPT_TS_ENABLED
    static_assert(mln::concepts::ConcreteImage<decltype(ima)>);
    static_assert(mln::concepts::OutputImage<decltype(ima)>);
    static_assert(mln::concepts::ViewImage<decltype(out)>);
    static_assert(mln::concepts::IndexableAndAccessibleImage<decltype(out)>);
    static_assert(mln::concepts::BidirectionalImage<decltype(out)>);
    static_assert(not mln::concepts::RawImage<decltype(out)>);
    static_assert(not mln::concepts::OutputImage<decltype(out)>);
#endif // PYLENE_CONCEPT_TS_ENABLED
    // Test pixel iteration
    // check that properties of pixels are preserved (point + index)
    {
      auto rng = mln::ranges::view::zip(ima.new_pixels(), out.new_pixels());
      mln_foreach_new ((auto [px1, px2]), rng)
      {
        ASSERT_EQ(px1.point(), px2.point());
        // ASSERT_EQ(px1.index(), px2.index());
        ASSERT_EQ(4 * px1.val(), px2.val());
        // ASSERT_EQ(&px2->image(), &out);
      }
    }

    // Test value iteration
    {
      mln_foreach_new ((auto [v1, v2]), mln::ranges::view::zip(ima.new_values(), out.new_values()))
        ASSERT_EQ(4 * v1, v2);
    }
  }
}

TEST(Core, Transform_Support_Writable)
{
  using namespace mln;

  typedef std::pair<int, int> V;

  box2d      dom{{-1, -2}, {3, 3}};
  image2d<V> ima(dom);
  {
    auto c1 = view::transform(ima, [](std::pair<int, int>& x) -> int& { return x.first; });
    auto c2 = view::transform(ima, [](const std::pair<int, int>& x) { return x.second; });
    fill(ima, std::make_pair(12, 12));

#ifdef PYLENE_CONCEPT_TS_ENABLED
    static_assert(mln::concepts::ConcreteImage<decltype(ima)>);
    static_assert(mln::concepts::OutputImage<decltype(ima)>);
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
#endif // PYLENE_CONCEPT_TS_ENABLED


    fill(c1, 69);

    // Does not compile because direct writing a temporary is forbidden by range-v3
    // fill(c2, 69);

    // Test pixel iteration
    // check that properties of pixels are preserved (point + index)
    {
      auto rng = mln::ranges::view::zip(ima.new_pixels(), c1.new_pixels(), c2.new_pixels());
      mln_foreach_new ((auto [px1, px2, px3]), rng)
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
      auto rng = mln::ranges::view::zip(ima.new_values(), c1.new_values(), c2.new_values());
      mln_foreach_new ((auto [v1, v2, v3]), rng)
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
  mln::box2d        dom{{-1, -2}, {3, 3}};
  mln::image2d<int> ima(dom, 3, 1);
  mln::image2d<int> ref(dom, 3, 2);

  auto c = mln::view::transform(ima, times_two);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::ConcreteImage<decltype(ima)>);
  static_assert(mln::concepts::OutputImage<decltype(ima)>);
  static_assert(mln::concepts::ViewImage<decltype(c)>);
  static_assert(mln::concepts::IndexableAndAccessibleImage<decltype(c)>);
  static_assert(mln::concepts::BidirectionalImage<decltype(c)>);
  static_assert(not mln::concepts::RawImage<decltype(c)>);
  static_assert(not mln::concepts::OutputImage<decltype(c)>);
#endif // PYLENE_CONCEPT_TS_ENABLED

  ASSERT_TRUE(mln::equal(ref, c));
}

TEST(Core, Transform_Supports_PointerToMemberFunction)
{
  using namespace mln::view::ops;

  using V = std::pair<int, int>;

  mln::box2d      dom{{-1, -2}, {3, 3}};
  mln::image2d<V> ima(dom, 3, std::make_pair(42, 42));
  mln::image2d<V> ref(dom, 3, std::make_pair(69, 42));

  auto c = mln::view::transform(ima, &V::first);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::ConcreteImage<decltype(ima)>);
  static_assert(mln::concepts::OutputImage<decltype(ima)>);
  static_assert(mln::concepts::ViewImage<decltype(c)>);
  static_assert(mln::concepts::IndexableAndAccessibleImage<decltype(c)>);
  static_assert(mln::concepts::BidirectionalImage<decltype(c)>);
  static_assert(not mln::concepts::RawImage<decltype(c)>);
  static_assert(mln::concepts::OutputImage<decltype(c)>);
#endif // PYLENE_CONCEPT_TS_ENABLED

  mln::fill(c, 69);
  ASSERT_TRUE(mln::equal(ima, ref));
}

TEST(Core, Transformed2Image_transform_byval_chain)
{
  using namespace mln;

  box2d        dom{{-1, -2}, {3, 3}};
  image2d<int> ima(dom);
  image2d<int> ima2(dom);

  iota(ima, 0);
  iota(ima2, 1);

  auto x   = view::transform(ima, [](int a) { return a > 3; });
  auto y   = view::transform(x, ima, [](bool a, int b) { return a ? b : 2; });
  auto out = view::transform(y, ima2, [](int a, int b) { return a + b; });

  std::vector<int> ref = {3, 4, 5, 6, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39};

  ASSERT_EQ(ref, ::ranges::to_vector(out.new_values()));
}
