#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/concepts/archetype/image.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/mask.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/range/foreach.hpp>

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

  static_assert(mln::concepts::OutputImage<decltype(z)>);
  static_assert(mln::concepts::ViewImage<decltype(z)>);
  static_assert(mln::concepts::IndexableAndAccessibleImage<decltype(z)>);
  static_assert(not mln::concepts::BidirectionalImage<decltype(z)>);
  static_assert(not mln::concepts::RawImage<decltype(z)>);

  auto dom = z.domain();
  for (auto&& r : dom.rows())
    for (auto p : r)
    {
      ASSERT_EQ(42, ima(p));
      ASSERT_EQ(42, z(p));
    }
  ASSERT_TRUE(mln::all_of(ima == ref));
}

template <mln::concepts::Image F>
void foo(F);

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

  static_assert(mln::concepts::OutputImage<decltype(A)>);
  static_assert(mln::concepts::ViewImage<decltype(A)>);
  static_assert(mln::concepts::IndexableAndAccessibleImage<decltype(A)>);
  static_assert(not mln::concepts::BidirectionalImage<decltype(A)>);
  static_assert(not mln::concepts::RawImage<decltype(A)>);


  auto mask_B = (A % 4) == 1;
  auto B      = mln::view::mask(A, mask_B);
  mln::fill(B, 42);

  static_assert(mln::concepts::OutputImage<decltype(B)>);
  static_assert(mln::concepts::ViewImage<decltype(B)>);
  static_assert(mln::concepts::IndexableAndAccessibleImage<decltype(B)>);
  static_assert(not mln::concepts::BidirectionalImage<decltype(B)>);
  static_assert(not mln::concepts::RawImage<decltype(B)>);

  auto dom = B.domain();
  for (auto&& r : dom.rows())
    for (auto p : r)
    {
      ASSERT_EQ(42, ima(p));
      ASSERT_EQ(42, B(p));
    }

  ASSERT_TRUE(mln::all_of(ima == ref));
}


struct mask_archetype : mln::details::Image<mask_archetype>
{
  using value_type    = bool;
  using reference     = const bool&;
  using domain_type   = mln::archetypes::Domain;
  using point_type    = ::ranges::range_value_t<domain_type>;
  using category_type = mln::forward_image_tag;
  using concrete_type = mask_archetype;

  struct pixel_type
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
  pixel_type pixel(point_type);
  pixel_type pixel_at(point_type);

  struct pixel_range
  {
    const pixel_type* begin();
    const pixel_type* end();
  };
  pixel_range pixels();


  struct value_range
  {
    const value_type* begin();
    const value_type* end();
  };

  value_range values();
};

/*
static_assert(mln::concepts::AccessibleImage<mln::mask_view<mln::archetypes::AccessibleImage, mask_archetype>>);
static_assert(mln::concepts::IndexableImage<mln::mask_view<mln::archetypes::IndexableImage, mask_archetype>>);
static_assert(mln::concepts::IndexableAndAccessibleImage<
              mln::mask_view<mln::archetypes::IndexableAndAccessibleImage, mask_archetype>>);
static_assert(mln::concepts::OutputImage<mln::mask_view<mln::archetypes::OutputAccessibleImage, mask_archetype>>);
static_assert(mln::concepts::OutputImage<mln::mask_view<mln::archetypes::OutputIndexableImage, mask_archetype>>);
static_assert(
    mln::concepts::OutputImage<mln::mask_view<mln::archetypes::OutputIndexableAndAccessibleImage, mask_archetype>>);
*/