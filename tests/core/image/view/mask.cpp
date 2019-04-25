#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/concept/new/archetype/image.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/view/mask.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/rangev3/foreach.hpp>

#include <gtest/gtest.h>


TEST(View, mask)
{
  using namespace mln::view::ops;

  mln::image2d<int> ima = {{0, 1, 2, 3, 4}, //
                           {5, 6, 7, 8, 9}, //
                           {10, 11, 12, 13, 14}};

  mln::image2d<int> ref = {{0, 42, 2, 42, 4},  //
                           {42, 6, 42, 8, 42}, //
                           {10, 42, 12, 42, 14}};


  auto mask = (ima % 2) == 1;


  auto z = mln::view::mask(ima, mask);
  fill(z, 42);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::OutputImage<decltype(z)>);
  static_assert(mln::concepts::ViewImage<decltype(z)>);
  static_assert(mln::concepts::IndexableAndAccessibleImage<decltype(z)>);
  static_assert(not mln::concepts::BidirectionalImage<decltype(z)>);
  static_assert(not mln::concepts::RawImage<decltype(z)>);
#endif // PYLENE_CONCEPT_TS_ENABLED

  for (auto p : z.domain())
  {
    ASSERT_EQ(42, ima(p));
    ASSERT_EQ(42, z(p));
  }
  ASSERT_TRUE(mln::all_of(ima == ref));
}


TEST(View, mask_twice)
{
  using namespace mln::view::ops;

  mln::image2d<int> ima = {{0, 1, 2, 3, 4}, //
                           {5, 6, 7, 8, 9}, //
                           {10, 11, 12, 13, 14}};

  mln::image2d<int> ref = {{0, 42, 2, 3, 4},  //
                           {42, 6, 7, 8, 42}, //
                           {10, 11, 12, 42, 14}};


  auto mask_A = (ima % 2) == 1;
  auto A      = mln::view::mask(ima, mask_A);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::OutputImage<decltype(A)>);
  static_assert(mln::concepts::ViewImage<decltype(A)>);
  static_assert(mln::concepts::IndexableAndAccessibleImage<decltype(A)>);
  static_assert(not mln::concepts::BidirectionalImage<decltype(A)>);
  static_assert(not mln::concepts::RawImage<decltype(A)>);
#endif // PYLENE_CONCEPT_TS_ENABLED

  auto mask_B = (A % 4) == 1;
  auto B      = mln::view::mask(A, mask_B);
  fill(B, 42);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::OutputImage<decltype(B)>);
  static_assert(mln::concepts::ViewImage<decltype(B)>);
  static_assert(mln::concepts::IndexableAndAccessibleImage<decltype(B)>);
  static_assert(not mln::concepts::BidirectionalImage<decltype(B)>);
  static_assert(not mln::concepts::RawImage<decltype(B)>);
#endif // PYLENE_CONCEPT_TS_ENABLED

  for (auto p : B.domain())
  {
    ASSERT_EQ(42, ima(p));
    ASSERT_EQ(42, B(p));
  }

  ASSERT_TRUE(mln::all_of(ima == ref));
}


struct mask_archetype : mln::experimental::Image<mask_archetype>
{
  using value_type    = bool;
  using reference     = const bool&;
  using domain_type   = mln::archetypes::Domain;
  using point_type    = ::ranges::range_value_t<domain_type>;
  using category_type = mln::forward_image_tag;
  using concrete_type = mask_archetype;

  struct new_pixel_type
  {
    bool       val() const;
    point_type point() const;
  };

  template <class V>
  using ch_value_type = mask_archetype;

  // additional traits
  using extension_category = mln::extension::none_extension_tag;
  using indexable          = std::false_type;
  using accessible         = std::true_type;
  using view               = std::false_type;


  domain_type    domain() const;
  reference      operator()(point_type);
  reference      at(point_type);
  new_pixel_type new_pixel(point_type);
  new_pixel_type new_pixel_at(point_type);

  struct pixel_range
  {
    const new_pixel_type* begin();
    const new_pixel_type* end();
  };
  pixel_range new_pixels();


  struct value_range
  {
    const value_type* begin();
    const value_type* end();
  };

  value_range new_values();
};


PYLENE_CONCEPT_TS_ASSERT(
    (mln::concepts::AccessibleImage<mln::mask_view<mln::archetypes::AccessibleImage, mask_archetype>>), "");
PYLENE_CONCEPT_TS_ASSERT(
    (mln::concepts::IndexableImage<mln::mask_view<mln::archetypes::IndexableImage, mask_archetype>>), "");
PYLENE_CONCEPT_TS_ASSERT((mln::concepts::IndexableAndAccessibleImage<
                             mln::mask_view<mln::archetypes::IndexableAndAccessibleImage, mask_archetype>>),
                         "");

PYLENE_CONCEPT_TS_ASSERT(
    (mln::concepts::OutputImage<mln::mask_view<mln::archetypes::OutputAccessibleImage, mask_archetype>>), "");
PYLENE_CONCEPT_TS_ASSERT(
    (mln::concepts::OutputImage<mln::mask_view<mln::archetypes::OutputIndexableImage, mask_archetype>>), "");
PYLENE_CONCEPT_TS_ASSERT(
    (mln::concepts::OutputImage<mln::mask_view<mln::archetypes::OutputIndexableAndAccessibleImage, mask_archetype>>),
    "");
