#include <mln/core/concept/new/archetype/image.hpp>
#include <mln/core/concept/new/archetype/pixel.hpp>
#include <mln/core/concept/new/pixels.hpp>
#include <mln/core/image/view/adaptor.hpp>

#include <gtest/gtest.h>


namespace concepts   = mln::concepts;
namespace archetypes = mln::archetypes;


TEST(Core, ViewAdaptor_Pixel)
{
#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(concepts::Pixel<mln::pixel_adaptor<archetypes::Pixel>>);
  static_assert(concepts::OutputPixel<mln::pixel_adaptor<archetypes::OutputPixel>>);
#endif // PYLENE_CONCEPT_TS_ENABLED
}


TEST(Core, ViewAdaptor_Image)
{
#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(concepts::Image<mln::image_adaptor<archetypes::Image>>);
  static_assert(concepts::IndexableImage<mln::image_adaptor<archetypes::IndexableImage>>);
  static_assert(concepts::AccessibleImage<mln::image_adaptor<archetypes::AccessibleImage>>);
  static_assert(concepts::IndexableAndAccessibleImage<mln::image_adaptor<archetypes::IndexableAndAccessibleImage>>);
  static_assert(concepts::BidirectionalImage<mln::image_adaptor<archetypes::BidirectionalImage>>);
  static_assert(concepts::RawImage<mln::image_adaptor<archetypes::RawImage>>);


  static_assert(concepts::Image<mln::image_adaptor<archetypes::OutputImage>>);
  static_assert(concepts::OutputImage<mln::image_adaptor<archetypes::OutputImage>>);
  static_assert(concepts::IndexableImage<mln::image_adaptor<archetypes::OutputIndexableImage>>);
  static_assert(concepts::OutputImage<mln::image_adaptor<archetypes::OutputIndexableImage>>);
  static_assert(concepts::AccessibleImage<mln::image_adaptor<archetypes::OutputAccessibleImage>>);
  static_assert(concepts::OutputImage<mln::image_adaptor<archetypes::OutputAccessibleImage>>);
  static_assert(
      concepts::IndexableAndAccessibleImage<mln::image_adaptor<archetypes::OutputIndexableAndAccessibleImage>>);
  static_assert(concepts::OutputImage<mln::image_adaptor<archetypes::OutputIndexableAndAccessibleImage>>);
  static_assert(concepts::BidirectionalImage<mln::image_adaptor<archetypes::OutputBidirectionalImage>>);
  static_assert(concepts::OutputImage<mln::image_adaptor<archetypes::OutputBidirectionalImage>>);
  static_assert(concepts::RawImage<mln::image_adaptor<archetypes::OutputRawImage>>);
  static_assert(concepts::OutputImage<mln::image_adaptor<archetypes::OutputRawImage>>);

  static_assert(concepts::WithExtensionImage<mln::image_adaptor<archetypes::WithExtensionImage>>);

  static_assert(not concepts::ConcreteImage<mln::image_adaptor<archetypes::Image>>);
  static_assert(concepts::ViewImage<mln::image_adaptor<archetypes::Image>>);
  static_assert(not concepts::ConcreteImage<mln::image_adaptor<archetypes::ConcreteImage>>);
  static_assert(concepts::ViewImage<mln::image_adaptor<archetypes::ConcreteImage>>);
#endif // PYLENE_CONCEPT_TS_ENABLED
}
