#include <mln/core/concept/new/archetype/image.hpp>
#include <mln/core/concept/new/archetype/pixel.hpp>
#include <mln/core/concept/new/pixels.hpp>
#include <mln/core/image/view/adaptor.hpp>

#include <gtest/gtest.h>


namespace concepts   = mln::concepts;
namespace archetypes = mln::archetypes;

template <class I>
struct identity : mln::image_adaptor<I>, mln::experimental::Image<identity<I>>
{
  using identity::image_adaptor::image_adaptor;
};


template <class Pix>
struct identity_pixel : mln::pixel_adaptor<Pix>, mln::experimental::Pixel<identity_pixel<Pix>>
{
  using identity_pixel::pixel_adaptor::pixel_adaptor;
};



TEST(Core, ViewAdaptor_Pixel)
{
#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(concepts::Pixel<identity_pixel<archetypes::Pixel>>);
  static_assert(concepts::OutputPixel<identity_pixel<archetypes::OutputPixel>>);
#endif // PYLENE_CONCEPT_TS_ENABLED
}


TEST(Core, ViewAdaptor_Image)
{
#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(concepts::Image<identity<archetypes::Image>>);
  static_assert(concepts::IndexableImage<identity<archetypes::IndexableImage>>);
  static_assert(concepts::AccessibleImage<identity<archetypes::AccessibleImage>>);
  static_assert(concepts::IndexableAndAccessibleImage<identity<archetypes::IndexableAndAccessibleImage>>);
  static_assert(concepts::BidirectionalImage<identity<archetypes::BidirectionalImage>>);
  static_assert(concepts::RawImage<identity<archetypes::RawImage>>);


  static_assert(concepts::Image<identity<archetypes::OutputImage>>);
  static_assert(concepts::OutputImage<identity<archetypes::OutputImage>>);
  static_assert(concepts::IndexableImage<identity<archetypes::OutputIndexableImage>>);
  static_assert(concepts::OutputImage<identity<archetypes::OutputIndexableImage>>);
  static_assert(concepts::AccessibleImage<identity<archetypes::OutputAccessibleImage>>);
  static_assert(concepts::OutputImage<identity<archetypes::OutputAccessibleImage>>);
  static_assert(
      concepts::IndexableAndAccessibleImage<identity<archetypes::OutputIndexableAndAccessibleImage>>);
  static_assert(concepts::OutputImage<identity<archetypes::OutputIndexableAndAccessibleImage>>);
  static_assert(concepts::BidirectionalImage<identity<archetypes::OutputBidirectionalImage>>);
  static_assert(concepts::OutputImage<identity<archetypes::OutputBidirectionalImage>>);
  static_assert(concepts::RawImage<identity<archetypes::OutputRawImage>>);
  static_assert(concepts::OutputImage<identity<archetypes::OutputRawImage>>);

  static_assert(concepts::WithExtensionImage<identity<archetypes::WithExtensionImage>>);

  static_assert(not concepts::ConcreteImage<identity<archetypes::Image>>);
  static_assert(concepts::ViewImage<identity<archetypes::Image>>);
  static_assert(not concepts::ConcreteImage<identity<archetypes::ConcreteImage>>);
  static_assert(concepts::ViewImage<identity<archetypes::ConcreteImage>>);
#endif // PYLENE_CONCEPT_TS_ENABLED
}
