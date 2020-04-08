#include <mln/core/image/view/filter.hpp>

#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>
#include <mln/core/algorithm/clone.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/rangev3/foreach.hpp>
#include <mln/core/rangev3/view/zip.hpp>

#include <mln/core/concept/new/archetype/image.hpp>

#include <gtest/gtest.h>


TEST(View, filter_readonly)
{
  using namespace mln::view::ops;

  mln::experimental::box2d        dom({-2, -1}, {3, 3});
  mln::experimental::image2d<int> ima(dom);

  mln::experimental::iota(ima, 0);
  auto x = mln::view::filter(ima, [](int v) { return v > 10; });

  static_assert(mln::concepts::OutputImage<decltype(x)>);
  static_assert(mln::concepts::Image<decltype(x)>);
  static_assert(mln::concepts::ViewImage<decltype(x)>);
  static_assert(mln::concepts::IndexableAndAccessibleImage<decltype(x)>);
  //static_assert(mln::concepts::BidirectionalImage<decltype(x)>); // FIXME
  static_assert(not mln::concepts::RawImage<decltype(x)>);


  ASSERT_TRUE(mln::all_of(x > 10));

  mln_foreach_new (auto&& pix, ima.new_pixels())
  {
    if (pix.val() > 10)
      ASSERT_EQ(pix.val(), x(pix.point()));
    else
      ASSERT_EQ(pix.val(), x.at(pix.point()));
  }

  mln_foreach_new (auto&& pix, x.new_pixels())
    ASSERT_EQ(pix.val(), ima(pix.point()));
}


TEST(View, filter_writable)
{
  using namespace mln::view::ops;

  mln::experimental::box2d        dom({-2, -1}, {3, 3});
  mln::experimental::image2d<int> ima(dom);


  mln::experimental::iota(ima, 0);
  auto x = mln::view::filter(ima, [](int v) { return v > 10; });

  static_assert(mln::concepts::OutputImage<decltype(x)>);
  static_assert(mln::concepts::ViewImage<decltype(x)>);
  static_assert(mln::concepts::IndexableAndAccessibleImage<decltype(x)>);
  // static_assert(mln::concepts::BidirectionalImage<decltype(x)>);
  static_assert(not mln::concepts::RawImage<decltype(x)>);


  mln::fill(x, 10);
  ASSERT_TRUE(mln::all_of(ima <= 10));
}


TEST(View, filter_twice)
{
  using namespace mln::view::ops;

  mln::experimental::box2d        dom({-2, -1}, {3, 3});
  mln::experimental::image2d<int> ima(dom);

  mln::experimental::iota(ima, 0);

  auto x = mln::view::filter(ima, [](int v) { return v > 10; });
  auto u = mln::view::filter(x, [](int v) { return v < 15; });


  static_assert(mln::concepts::OutputImage<decltype(u)>);
  static_assert(mln::concepts::ViewImage<decltype(u)>);
  static_assert(mln::concepts::IndexableAndAccessibleImage<decltype(u)>);
  // static_assert(mln::concepts::BidirectionalImage<decltype(u)>);
  static_assert(not mln::concepts::RawImage<decltype(u)>);


  ASSERT_TRUE(mln::all_of(u > 10 && u < 15));

  mln_foreach_new (auto&& pix, ima.new_pixels())
  {
    if (pix.val() > 10 and pix.val() < 15)
    {
      ASSERT_EQ(pix.val(), u(pix.point()));
      ASSERT_TRUE(u.domain().has(pix.point()));
    }
    else
    {
      ASSERT_EQ(pix.val(), u.at(pix.point()));
      ASSERT_FALSE(u.domain().has(pix.point()));
    }
  }

  {
    mln_foreach_new (auto&& pix, u.new_pixels())
      ASSERT_EQ(pix.val(), ima(pix.point()));
  }

  mln::experimental::image2d<int> before = mln::clone(ima);
  mln::fill(u, 1);
  {
    mln_foreach_new ((auto [old_v, new_v]), mln::ranges::view::zip(before.new_values(), ima.new_values()))
    {
      if (old_v > 10 && old_v < 15)
        ASSERT_EQ(1, new_v);
      else
        ASSERT_EQ(old_v, new_v);
    }
  }

  mln_foreach_new (auto&& px, u.new_pixels())
    ASSERT_EQ(px.val(), ima(px.point()));
}


struct archetype_pred_t
{
  template <typename T>
  bool operator()(T&&) const;
};

static_assert(
    (mln::concepts::AccessibleImage<mln::filter_view<mln::archetypes::AccessibleImage, archetype_pred_t>>), "");
static_assert(
    (mln::concepts::IndexableImage<mln::filter_view<mln::archetypes::IndexableAndAccessibleImage, archetype_pred_t>>),
    "");
static_assert((mln::concepts::IndexableAndAccessibleImage<
                             mln::filter_view<mln::archetypes::IndexableAndAccessibleImage, archetype_pred_t>>),
                         "");
static_assert((mln::concepts::OutputImage<
                             mln::filter_view<mln::archetypes::OutputIndexableAndAccessibleImage, archetype_pred_t>>),
                         "");
