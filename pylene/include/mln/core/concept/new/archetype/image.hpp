#pragma once

#include <mln/core/concept/new/archetype/domain.hpp>
#include <mln/core/concept/new/archetype/pixel.hpp>
#include <mln/core/concept/new/images.hpp>

#include <mln/core/domain/private/domain_traits.hpp>
#include <mln/core/image/private/image_traits.hpp>
#include <mln/core/image/private/pixel_traits.hpp>

#include <type_traits>


namespace mln::archetypes
{
  struct Image;

  struct Image
  {
    using new_pixel_type = Pixel;
    using value_type = pixel_value_t<Pixel>;
    using reference  = pixel_reference_t<Pixel>;
    using new_point_type = domain_point_t<Domain>;
    using domain_type    = Domain;
    using category_type  = forward_image_tag;
    using concrete_type  = Image;

#ifdef PYLENE_CONCEPT_TS_ENABLED
    template <mln::concepts::Value Val>
#else
    template <typename Val>
#endif // PYLENE_CONCEPT_TS_ENABLED
    using ch_value_type = Image;

    // additional traits
    using extension_category = mln::extension::none_extension_tag;
    using indexable          = std::false_type;
    using accessible         = std::false_type;


#ifdef PYLENE_CONCEPT_TS_ENABLED
    template <mln::concepts::Value Val>
#else
    template <typename Val>
#endif // PYLENE_CONCEPT_TS_ENABLED
    ch_value_type<Val> ch_value() const;
    concrete_type      concretize() const;
    domain_type        domain() const;


    struct PixelRange
    {
      using value_type = image_pixel_t<Image>;
      using reference  = image_pixel_t<Image>&;

      new_pixel_type* begin();
      new_pixel_type* end();
    };

    PixelRange new_pixels();


    struct ValueRange
    {
      using value_type = image_value_t<Image>;
      using reference  = image_reference_t<Image>;

      image_value_t<Image>* begin();
      image_value_t<Image>* end();
    };

    ValueRange new_values();
  };

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::Image<Image>, "Image archetype does not model the Image concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED


  struct OutputImage : Image
  {
    using new_pixel_type = OutputPixel;
    using reference  = pixel_reference_t<OutputPixel>;


    struct OutputPixelRange final
    {
      using value_type = image_pixel_t<OutputImage>;
      using reference  = image_pixel_t<OutputImage>&;

      value_type* begin();
      value_type* end();
    };

    OutputPixelRange new_pixels();
  };

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::Image<OutputImage> && mln::concepts::OutputImage<OutputImage>,
                "OuputImage archetype does not model the OuputImage concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED


  struct InputImage : Image
  {
  };

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::InputImage<InputImage>, "InputImage archetype does not model the InputImage concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED


  struct ForwardImage final : InputImage
  {
  };

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::ForwardImage<ForwardImage>,
                "ForwardImage archetype does not model the ForwardImage concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED


  struct OutputForwardImage final : OutputImage
  {
  };

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::ForwardImage<OutputForwardImage> && mln::concepts::OutputImage<OutputForwardImage>,
                "OutputForwardImage archetype does not model the OutputImage concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED


  struct IndexableImage : Image
  {
    using index_type = int;
    using indexable  = std::true_type;

    image_reference_t<IndexableImage> operator[](image_index_t<IndexableImage>) const;
    image_point_t<IndexableImage>     new_point_at_index(image_index_t<IndexableImage>) const;
    image_index_t<IndexableImage>     new_index_of_point(image_point_t<IndexableImage>) const;
    image_index_t<IndexableImage>     delta_index(image_point_t<IndexableImage>) const;
  };

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::IndexableImage<IndexableImage>,
                "IndexableImage archetype does not model the IndexableImage concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED


  struct OutputIndexableImage final : IndexableImage
  {
    using new_pixel_type = OutputPixel;
    using reference  = pixel_reference_t<OutputPixel>;

    image_reference_t<OutputIndexableImage> operator[](image_index_t<OutputIndexableImage>) const;
    image_reference_t<OutputIndexableImage> operator[](image_index_t<OutputIndexableImage>);


    struct OutputPixelRange final
    {
      using value_type = image_pixel_t<OutputIndexableImage>;
      using reference  = image_pixel_t<OutputIndexableImage>&;

      value_type* begin();
      value_type* end();
    };

    OutputPixelRange new_pixels();
  };


#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::IndexableImage<OutputIndexableImage> && mln::concepts::OutputImage<OutputIndexableImage>,
                "OutputIndexableImage archetype does not model the OutputImage concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED


  struct AccessibleImage : Image
  {
    using accessible = std::true_type;

    image_reference_t<AccessibleImage> operator()(image_point_t<AccessibleImage>) const;
    image_reference_t<AccessibleImage> at(image_point_t<AccessibleImage>) const;
    image_pixel_t<AccessibleImage>     new_pixel(image_point_t<AccessibleImage>) const;
    image_pixel_t<AccessibleImage>     new_pixel_at(image_point_t<AccessibleImage>) const;
  };

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::AccessibleImage<AccessibleImage>,
                "AccessibleImage archetype does not model the AccessibleImage concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED


  struct OutputAccessibleImage final : AccessibleImage
  {
    using new_pixel_type = OutputPixel;
    using reference  = pixel_reference_t<OutputPixel>;

    image_reference_t<OutputAccessibleImage> operator()(image_point_t<OutputAccessibleImage>) const;
    image_reference_t<OutputAccessibleImage> operator()(image_point_t<OutputAccessibleImage>);

    image_reference_t<OutputAccessibleImage> at(image_point_t<OutputAccessibleImage>) const;
    image_reference_t<OutputAccessibleImage> at(image_point_t<OutputAccessibleImage>);

    image_pixel_t<OutputAccessibleImage> new_pixel(image_point_t<OutputAccessibleImage>) const;
    image_pixel_t<OutputAccessibleImage> new_pixel_at(image_point_t<OutputAccessibleImage>) const;

    struct OutputPixelRange final
    {
      using value_type = image_pixel_t<OutputAccessibleImage>;
      using reference  = image_pixel_t<OutputAccessibleImage>&;

      value_type* begin();
      value_type* end();
    };

    OutputPixelRange new_pixels();
  };

#ifdef PYLENE_CONCEPT_TS_ENABLED

  static_assert(mln::concepts::AccessibleImage<OutputAccessibleImage> &&
                    mln::concepts::OutputImage<OutputAccessibleImage>,
                "OutputAccessibleImage archetype does not model the OutputImage concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED


  struct BidirectionalImage : Image
  {
    using category_type = bidirectional_image_tag;

    struct ReversiblePixelRange final : Image::PixelRange
    {
      Image::PixelRange reversed();
    };

    ReversiblePixelRange new_pixels();

    struct ReversibleValueRange final : Image::ValueRange
    {
      Image::ValueRange reversed();
    };

    ReversibleValueRange new_values();
  };

#ifdef PYLENE_CONCEPT_TS_ENABLED

  static_assert(mln::concepts::BidirectionalImage<BidirectionalImage>,
                "BidirectionalImage archetype does not model the BidirectionalImage concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED


  struct OutputBidirectionalImage : BidirectionalImage
  {
    using new_pixel_type = OutputPixel;
    using reference  = pixel_reference_t<OutputPixel>;

    struct OutputPixelRange
    {
      using value_type = image_pixel_t<OutputBidirectionalImage>;
      using reference  = image_pixel_t<OutputBidirectionalImage>&;

      value_type* begin();
      value_type* end();
    };

    struct ReversibleOutputPixelRange final : OutputPixelRange
    {
      OutputPixelRange reversed();
    };

    ReversibleOutputPixelRange new_pixels();
  };

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::BidirectionalImage<OutputBidirectionalImage> &&
                    mln::concepts::OutputImage<OutputBidirectionalImage>,
                "OutputBidirectionalImage archetype does not model the OutputImage concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED


  struct RawImage : Image
  {
    using category_type = raw_image_tag;
    using index_type    = int;
    using indexable     = std::true_type;
    using accessible    = std::true_type;

    image_reference_t<RawImage> operator[](image_index_t<RawImage>) const;
    image_point_t<RawImage>     new_point_at_index(image_index_t<RawImage>) const;
    image_index_t<RawImage>     new_index_of_point(image_point_t<RawImage>) const;
    image_index_t<RawImage>     delta_index(image_point_t<RawImage>) const;

    image_reference_t<RawImage> operator()(image_point_t<RawImage>) const;
    image_reference_t<RawImage> at(image_point_t<RawImage>) const;
    image_pixel_t<RawImage>     new_pixel(image_point_t<RawImage>) const;
    image_pixel_t<RawImage>     new_pixel_at(image_point_t<RawImage>) const;


    struct ReversiblePixelRange final : Image::PixelRange
    {
      Image::PixelRange reversed();
    };

    ReversiblePixelRange new_pixels();

    struct ReversibleValueRange final : Image::ValueRange
    {
      Image::ValueRange reversed();
    };

    ReversibleValueRange new_values();


    const image_value_t<RawImage>* data() const;
    std::size_t                    strides(int) const;
  };

#ifdef PYLENE_CONCEPT_TS_ENABLED

  static_assert(mln::concepts::RawImage<RawImage>, "RawImage archetype does not model the RawImage concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED


  struct OutputRawImage final : RawImage
  {
    using new_pixel_type = OutputPixel;
    using reference  = pixel_reference_t<OutputPixel>;

    image_reference_t<OutputRawImage> operator[](image_index_t<OutputRawImage>) const;
    image_reference_t<OutputRawImage> operator[](image_index_t<OutputRawImage>);

    image_reference_t<OutputRawImage> operator()(image_point_t<OutputRawImage>) const;
    image_reference_t<OutputRawImage> operator()(image_point_t<OutputRawImage>);

    image_reference_t<OutputRawImage> at(image_point_t<OutputRawImage>) const;
    image_reference_t<OutputRawImage> at(image_point_t<OutputRawImage>);

    // Need to be redefined as pixel_type has changed
    image_pixel_t<OutputRawImage> new_pixel(image_point_t<OutputRawImage>) const;
    image_pixel_t<OutputRawImage> new_pixel_at(image_point_t<OutputRawImage>) const;


    struct OutputPixelRange
    {
      using value_type = image_pixel_t<OutputRawImage>;
      using reference  = image_pixel_t<OutputRawImage>&;

      value_type* begin();
      value_type* end();
    };

    struct ReversibleOutputPixelRange final : OutputPixelRange
    {
      OutputPixelRange reversed();
    };

    ReversibleOutputPixelRange new_pixels();


    using RawImage::data;
    image_value_t<OutputRawImage>* data();
  };

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::RawImage<OutputRawImage> && mln::concepts::OutputImage<OutputRawImage>,
                "OutputRawImage archetype does not model the OutputImage concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED


  struct ExtendedImage : Image
  {
    struct Extension
    {
    };

    using extension_type = Extension;

    using extension_category = mln::extension::custom_extension_tag;

    image_extension_t<ExtendedImage> extension() const;
  };

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::ExtendedImage<ExtendedImage>,
                "ExtendedImage archetype does not model the ExtendedImage concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED
} // namespace mln::archetypes
