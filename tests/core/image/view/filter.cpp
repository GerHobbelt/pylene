#include <mln/core/image/image2d.hpp>
#include <mln/core/image/private/image_operators.hpp>
#include <mln/core/image/view/filter.hpp>

#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/iota.hpp>

#include <mln/core/rangev3/foreach.hpp>
#include <mln/core/rangev3/view/zip.hpp>

#include <mln/core/concept/new/concepts.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>

#include <gtest/gtest.h>


TEST(View, filter_readonly)
{
  using namespace mln::experimental::ops;

  mln::box2d        dom = {{-1, -2}, {3, 3}};
  mln::image2d<int> ima(dom);

  mln::experimental::iota(ima, 0);
  auto x = mln::view::filter(ima, [](int v) { return v > 10; });

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(concepts::OutputImage<decltype(x)>);
  static_assert(concepts::Image<decltype(x)>);
  static_assert(concepts::ViewImage<decltype(x)>);
  static_assert(concepts::IndexableAndAccessibleImage<decltype(x)>);
  static_assert(concepts::BidirectionalImage<decltype(x)>);
  static_assert(not concepts::RawImage<decltype(x)>);
#endif // PYLENE_CONCEPT_TS_ENABLED


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
  using namespace mln::experimental::ops;

  mln::box2d        dom{{-1, -2}, {3, 3}};
  mln::image2d<int> ima(dom);


  mln::experimental::iota(ima, 0);
  auto x = mln::view::filter(ima, [](int v) { return v > 10; });

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(concepts::OutputImage<decltype(x)>);
  static_assert(concepts::ViewImage<decltype(x)>);
  static_assert(concepts::IndexableAndAccessibleImage<decltype(x)>);
  static_assert(concepts::BidirectionalImage<decltype(x)>);
  static_assert(not concepts::RawImage<decltype(x)>);
#endif // PYLENE_CONCEPT_TS_ENABLED


  mln::fill(x, 10);
  ASSERT_TRUE(mln::all_of(ima <= 10));
}


TEST(View, filter_twice)
{
  using namespace mln::experimental::ops;

  mln::box2d        dom = {{-1, -2}, {3, 3}};
  mln::image2d<int> ima(dom);

  mln::experimental::iota(ima, 0);

  auto x = mln::view::filter(ima, [](int v) { return v > 10; });
  auto u = mln::view::filter(x, [](int v) { return v < 15; });

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(concepts::OutputImage<decltype(u)>);
  static_assert(concepts::ViewImage<decltype(u)>);
  static_assert(concepts::IndexableAndAccessibleImage<decltype(u)>);
  static_assert(concepts::BidirectionalImage<decltype(u)>);
  static_assert(not concepts::RawImage<decltype(u)>);
#endif // PYLENE_CONCEPT_TS_ENABLED

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

  mln::image2d<int> before = mln::clone(ima);
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

PYLENE_CONCEPT_TS_ASSERT(
    (mln::concepts::AccessibleImage<mln::filter_view<mln::archetypes::AccessibleImage, archetype_pred_t>>), "");
PYLENE_CONCEPT_TS_ASSERT(
    (mln::concepts::IndexableImage<mln::filter_view<mln::archetypes::IndexableAndAccessibleImage, archetype_pred_t>>),
    "");
PYLENE_CONCEPT_TS_ASSERT((mln::concepts::IndexableAndAccessibleImage<
                             mln::filter_view<mln::archetypes::IndexableAndAccessibleImage, archetype_pred_t>>),
                         "");
PYLENE_CONCEPT_TS_ASSERT((mln::concepts::OutputImage<
                             mln::filter_view<mln::archetypes::OutputIndexableAndAccessibleImage, archetype_pred_t>>),
                         "");
