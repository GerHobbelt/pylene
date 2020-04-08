#include <mln/core/algorithm/iota.hpp>
#include <mln/core/box.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/image/view/value_extended.hpp>
#include <mln/core/image/view/zip.hpp>
#include <mln/core/neighborhood/c8.hpp>
#include <mln/core/rangev3/foreach.hpp>

#include <mln/core/concept/archetype/image.hpp>

#include <gtest/gtest.h>

static_assert(mln::concepts::InputImage<mln::value_extended_view<mln::archetypes::InputImage>>);
static_assert(mln::concepts::OutputImage<mln::value_extended_view<mln::archetypes::OutputImage>>);
static_assert(mln::concepts::IndexableImage<mln::value_extended_view<mln::archetypes::IndexableImage>>);
static_assert(mln::concepts::AccessibleImage<mln::value_extended_view<mln::archetypes::AccessibleImage>>);
static_assert(mln::concepts::BidirectionalImage<mln::value_extended_view<mln::archetypes::BidirectionalImage>>);
static_assert(!mln::concepts::RawImage<mln::value_extended_view<mln::archetypes::RawImage>>);



TEST(View, value_extended)
{
  mln::experimental::box2d        dom{{-1, -2}, {3, 3}};
  mln::experimental::image2d<int> ima(dom);

  mln::iota(ima, 0);
  auto x = mln::view::value_extended(ima, 69);

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
          ASSERT_EQ(q.val(), ima(q.point()));
  }
}
