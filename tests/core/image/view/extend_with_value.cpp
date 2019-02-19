#include <mln/core/algorithm/iota.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/view/extend_with_value.hpp>
#include <mln/core/image/view/zip.hpp>
#include <mln/core/neighborhood/c8.hpp>
#include <mln/core/rangev3/foreach.hpp>


#include <gtest/gtest.h>

#ifdef PYLENE_CONCEPT_TS_ENABLED
static_assert(mln::concepts::InputImage<mln::extended_by_value_view<mln::archetypes::InputImage>>);
static_assert(mln::concepts::OutputImage<mln::extended_by_value_view<mln::archetypes::OutputImage>>);
static_assert(mln::concepts::IndexableImage<mln::extended_by_value_view<mln::archetypes::IndexableImage>>);
static_assert(mln::concepts::AccessibleImage<mln::extended_by_value_view<mln::archetypes::AccessibleImage>>);
static_assert(mln::concepts::BidirectionalImage<mln::extended_by_value_view<mln::archetypes::BidirectionalImage>>);
static_assert(!mln::concepts::RawImage<mln::extended_by_value_view<mln::archetypes::RawImage>>);
#endif


TEST(View, extend_with_value)
{
  mln::box2d        dom{{-1, -2}, {3, 3}};
  mln::image2d<int> ima(dom);

  mln::experimental::iota(ima, 0);
  auto x = mln::view::extend_with_value(ima, 69);

  {
    auto zz = mln::view::zip(ima, x);
    mln_foreach_new(auto px, zz.new_pixels())
    {
      auto [v1,v2] = px.val();
      ASSERT_EQ(v1, v2);
    }
  }

  {
    mln_foreach_new(auto p, x.new_pixels())
      for (auto q : mln::experimental::c8(p))
        if (!x.domain().has(q.point()))
          ASSERT_EQ(q.val(), 69);
        else
          ASSERT_EQ(&q.val(), &ima(q.point()));
  }

}
