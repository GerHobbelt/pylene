#include <mln/core/concepts/archetype/image.hpp>
#include <mln/core/concepts/archetype/pixel.hpp>
#include <mln/core/concepts/pixel.hpp>
#include <mln/core/image/view/adaptor.hpp>

#include <gtest/gtest.h>


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
  static_assert(mln::concepts::Pixel<identity_pixel<mln::archetypes::Pixel>>);
  static_assert(mln::concepts::OutputPixel<identity_pixel<mln::archetypes::OutputPixel>>);
}


TEST(Core, ViewAdaptor_Image)
{
  static_assert(mln::concepts::Image<identity<mln::archetypes::Image>>);
  static_assert(mln::concepts::IndexableImage<identity<mln::archetypes::IndexableImage>>);
  static_assert(mln::concepts::AccessibleImage<identity<mln::archetypes::AccessibleImage>>);
  static_assert(mln::concepts::IndexableAndAccessibleImage<identity<mln::archetypes::IndexableAndAccessibleImage>>);
  static_assert(mln::concepts::BidirectionalImage<identity<mln::archetypes::BidirectionalImage>>);
  static_assert(mln::concepts::RawImage<identity<mln::archetypes::RawImage>>);


  static_assert(mln::concepts::Image<identity<mln::archetypes::OutputImage>>);
  static_assert(mln::concepts::OutputImage<identity<mln::archetypes::OutputImage>>);
  static_assert(mln::concepts::IndexableImage<identity<mln::archetypes::OutputIndexableImage>>);
  static_assert(mln::concepts::OutputImage<identity<mln::archetypes::OutputIndexableImage>>);
  static_assert(mln::concepts::AccessibleImage<identity<mln::archetypes::OutputAccessibleImage>>);
  static_assert(mln::concepts::OutputImage<identity<mln::archetypes::OutputAccessibleImage>>);
  static_assert(
      mln::concepts::IndexableAndAccessibleImage<identity<mln::archetypes::OutputIndexableAndAccessibleImage>>);
  static_assert(mln::concepts::OutputImage<identity<mln::archetypes::OutputIndexableAndAccessibleImage>>);
  static_assert(mln::concepts::BidirectionalImage<identity<mln::archetypes::OutputBidirectionalImage>>);
  static_assert(mln::concepts::OutputImage<identity<mln::archetypes::OutputBidirectionalImage>>);
  static_assert(mln::concepts::RawImage<identity<mln::archetypes::OutputRawImage>>);
  static_assert(mln::concepts::OutputImage<identity<mln::archetypes::OutputRawImage>>);

  static_assert(mln::concepts::WithExtensionImage<identity<mln::archetypes::WithExtensionImage>>);

  static_assert(not mln::concepts::ConcreteImage<identity<mln::archetypes::Image>>);
  static_assert(mln::concepts::ViewImage<identity<mln::archetypes::Image>>);
  static_assert(not mln::concepts::ConcreteImage<identity<mln::archetypes::ConcreteImage>>);
  static_assert(mln::concepts::ViewImage<identity<mln::archetypes::ConcreteImage>>);
}
