#pragma once

#include <mln/core/concept/new/archetype/domain.hpp>
#include <mln/core/concept/new/archetype/pixel.hpp>
#include <mln/core/concept/new/check.hpp>
#include <mln/core/concept/new/images.hpp>

#include <mln/core/domain/private/domain_traits.hpp>
#include <mln/core/image/private/image_traits.hpp>
#include <mln/core/image/private/pixel_traits.hpp>

#include <type_traits>


namespace mln::archetypes
{


  struct ConcreteImage;


  struct ConcreteImage : mln::experimental::Image<ConcreteImage>
  {
    using new_pixel_type = Pixel;
    using value_type     = pixel_value_t<Pixel>;
    using reference      = pixel_reference_t<Pixel>;
    using point_type     = domain_point_t<Domain>;
    using domain_type    = Domain;
    using category_type  = forward_image_tag;
    using concrete_type  = ConcreteImage;

#ifdef PYLENE_CONCEPT_TS_ENABLED
    template <mln::concepts::Value Val>
#else
    template <typename Val>
#endif // PYLENE_CONCEPT_TS_ENABLED
    using ch_value_type = ConcreteImage;

    // additional traits
    using extension_category = mln::extension::none_extension_tag;
    using indexable          = std::false_type;
    using accessible         = std::false_type;
    using view               = std::false_type;

    ConcreteImage()                     = default;
    ConcreteImage(const ConcreteImage&) = default;
    ConcreteImage(ConcreteImage&&)      = default;
    ConcreteImage& operator=(const ConcreteImage&) = default;
    ConcreteImage& operator=(ConcreteImage&&) = default;

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
      using value_type = image_pixel_t<ConcreteImage>;
      using reference  = image_pixel_t<ConcreteImage>&;

      new_pixel_type* begin();
      new_pixel_type* end();
    };

    PixelRange new_pixels();


    struct ValueRange
    {
      using value_type = image_value_t<ConcreteImage>;
      using reference  = image_reference_t<ConcreteImage>;

      image_value_t<ConcreteImage>* begin();
      image_value_t<ConcreteImage>* end();
    };

    ValueRange new_values();
  };

  PYLENE_CONCEPT_TS_ASSERT(mln::concepts::ConcreteImage<ConcreteImage>,
                           "ConcreteImage archetype does not model the ConcreteImage concept!");


  struct ViewImage : ConcreteImage, mln::experimental::Image<ViewImage>
  {
    using view = std::true_type;

    ViewImage()                 = delete;
    ViewImage(const ViewImage&) = default;
    ViewImage(ViewImage&&)      = default;
    ViewImage& operator=(const ViewImage&) = delete;
    ViewImage& operator=(ViewImage&&) = delete;
  };

  PYLENE_CONCEPT_TS_ASSERT(mln::concepts::ViewImage<ViewImage>,
                           "ViewImage archetype does not model the ViewImage concept!")


  using Image = ViewImage;

  PYLENE_CONCEPT_TS_ASSERT(mln::concepts::Image<Image>, "Image archetype does not model the Image concept!");


  struct OutputImage : Image, mln::experimental::Image<OutputImage>
  {
    using new_pixel_type = OutputPixel;
    using reference      = pixel_reference_t<OutputPixel>;


    struct OutputPixelRange final
    {
      using value_type = image_pixel_t<OutputImage>;
      using reference  = image_pixel_t<OutputImage>&;

      value_type* begin();
      value_type* end();
    };

    OutputPixelRange new_pixels();
  };

  PYLENE_CONCEPT_TS_ASSERT(mln::concepts::Image<OutputImage>&& mln::concepts::OutputImage<OutputImage>,
                           "OuputImage archetype does not model the OuputImage concept!");


  using InputImage = Image;

  PYLENE_CONCEPT_TS_ASSERT(mln::concepts::InputImage<InputImage>,
                           "InputImage archetype does not model the InputImage concept!");


  using ForwardImage = InputImage;

  PYLENE_CONCEPT_TS_ASSERT(mln::concepts::ForwardImage<ForwardImage>,
                           "ForwardImage archetype does not model the ForwardImage concept!");


  using OutputForwardImage = OutputImage;

  PYLENE_CONCEPT_TS_ASSERT(
      mln::concepts::ForwardImage<OutputForwardImage>&& mln::concepts::OutputImage<OutputForwardImage>,
      "OutputForwardImage archetype does not model the OutputImage concept!");


  struct IndexableImage : Image, mln::experimental::Image<IndexableImage>
  {
    using index_type = int;
    using indexable  = std::true_type;

    image_reference_t<IndexableImage> operator[](image_index_t<IndexableImage>) const;
  };

  PYLENE_CONCEPT_TS_ASSERT(mln::concepts::IndexableImage<IndexableImage>,
                           "IndexableImage archetype does not model the IndexableImage concept!");


  struct OutputIndexableImage final : IndexableImage, mln::experimental::Image<OutputIndexableImage>
  {
    using new_pixel_type = OutputPixel;
    using reference      = pixel_reference_t<OutputPixel>;

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


  PYLENE_CONCEPT_TS_ASSERT(
      mln::concepts::IndexableImage<OutputIndexableImage>&& mln::concepts::OutputImage<OutputIndexableImage>,
      "OutputIndexableImage archetype does not model the OutputImage concept!");


  struct AccessibleImage : Image, mln::experimental::Image<AccessibleImage>
  {
    using accessible = std::true_type;

    image_reference_t<AccessibleImage> operator()(image_point_t<AccessibleImage>) const;
    image_reference_t<AccessibleImage> at(image_point_t<AccessibleImage>) const;
    image_pixel_t<AccessibleImage>     new_pixel(image_point_t<AccessibleImage>) const;
    image_pixel_t<AccessibleImage>     new_pixel_at(image_point_t<AccessibleImage>) const;
  };

  PYLENE_CONCEPT_TS_ASSERT(mln::concepts::AccessibleImage<AccessibleImage>,
                           "AccessibleImage archetype does not model the AccessibleImage concept!");


  struct OutputAccessibleImage : AccessibleImage, mln::experimental::Image<OutputAccessibleImage>
  {
    using new_pixel_type = OutputPixel;
    using reference      = pixel_reference_t<OutputPixel>;

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

  PYLENE_CONCEPT_TS_ASSERT(
      mln::concepts::AccessibleImage<OutputAccessibleImage>&& mln::concepts::OutputImage<OutputAccessibleImage>,
      "OutputAccessibleImage archetype does not model the OutputImage concept!");


  struct IndexableAndAccessibleImage : AccessibleImage, mln::experimental::Image<IndexableAndAccessibleImage>
  {
    using index_type = int;
    using indexable  = std::true_type;

    image_reference_t<IndexableAndAccessibleImage> operator[](image_index_t<IndexableAndAccessibleImage>) const;

    image_point_t<IndexableAndAccessibleImage> point_at_index(image_index_t<IndexableAndAccessibleImage>) const;
    image_index_t<IndexableAndAccessibleImage> index_of_point(image_point_t<IndexableAndAccessibleImage>) const;
    image_index_t<IndexableAndAccessibleImage> delta_index(image_point_t<IndexableAndAccessibleImage>) const;
  };

  PYLENE_CONCEPT_TS_ASSERT(
      mln::concepts::IndexableAndAccessibleImage<IndexableAndAccessibleImage>,
      "IndexableAndAccessibleImage archetype does not model the IndexableAndAccessibleImage concept!");


  struct OutputIndexableAndAccessibleImage : OutputAccessibleImage,
                                             mln::experimental::Image<OutputIndexableAndAccessibleImage>
  {
    using index_type = int;
    using indexable  = std::true_type;

    image_reference_t<OutputIndexableAndAccessibleImage>
                                                         operator[](image_index_t<OutputIndexableAndAccessibleImage>) const;
    image_reference_t<OutputIndexableAndAccessibleImage> operator[](image_index_t<OutputIndexableAndAccessibleImage>);

    image_point_t<OutputIndexableAndAccessibleImage>
        point_at_index(image_index_t<OutputIndexableAndAccessibleImage>) const;
    image_index_t<OutputIndexableAndAccessibleImage>
        index_of_point(image_point_t<OutputIndexableAndAccessibleImage>) const;
    image_index_t<OutputIndexableAndAccessibleImage>
        delta_index(image_point_t<OutputIndexableAndAccessibleImage>) const;
  };

  PYLENE_CONCEPT_TS_ASSERT(mln::concepts::IndexableAndAccessibleImage<OutputIndexableAndAccessibleImage>&&
                               mln::concepts::OutputImage<OutputIndexableAndAccessibleImage>,
                           "OutputIndexableAndAccessibleImage archetype does not model the OutputImage concept!");


  struct BidirectionalImage : Image, mln::experimental::Image<BidirectionalImage>
  {
    using category_type = bidirectional_image_tag;

    struct ReversiblePixelRange final : mln::archetypes::Image::PixelRange
    {
      mln::archetypes::Image::PixelRange reversed();
    };

    ReversiblePixelRange new_pixels();

    struct ReversibleValueRange final : mln::archetypes::Image::ValueRange
    {
      mln::archetypes::Image::ValueRange reversed();
    };

    ReversibleValueRange new_values();
  };

  PYLENE_CONCEPT_TS_ASSERT(mln::concepts::BidirectionalImage<BidirectionalImage>,
                           "BidirectionalImage archetype does not model the BidirectionalImage concept!");


  struct OutputBidirectionalImage : BidirectionalImage, mln::experimental::Image<OutputBidirectionalImage>
  {
    using new_pixel_type = OutputPixel;
    using reference      = pixel_reference_t<OutputPixel>;

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

  PYLENE_CONCEPT_TS_ASSERT(mln::concepts::BidirectionalImage<OutputBidirectionalImage>&&
                               mln::concepts::OutputImage<OutputBidirectionalImage>,
                           "OutputBidirectionalImage archetype does not model the OutputImage concept!");


  struct RawImage : IndexableAndAccessibleImage, mln::experimental::Image<RawImage>
  {
    using category_type = raw_image_tag;


    struct ReversiblePixelRange final : mln::archetypes::Image::PixelRange
    {
      mln::archetypes::Image::PixelRange reversed();
    };

    ReversiblePixelRange new_pixels();

    struct ReversibleValueRange final : mln::archetypes::Image::ValueRange
    {
      mln::archetypes::Image::ValueRange reversed();
    };

    ReversibleValueRange new_values();


    const image_value_t<RawImage>* data() const;
    std::ptrdiff_t                 strides(int) const;
  };

  PYLENE_CONCEPT_TS_ASSERT(mln::concepts::RawImage<RawImage>,
                           "RawImage archetype does not model the RawImage concept!");


  struct OutputRawImage final : RawImage, mln::experimental::Image<OutputRawImage>
  {
    using new_pixel_type = OutputPixel;
    using reference      = pixel_reference_t<OutputPixel>;

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

  PYLENE_CONCEPT_TS_ASSERT(mln::concepts::RawImage<OutputRawImage>&& mln::concepts::OutputImage<OutputRawImage>,
                           "OutputRawImage archetype does not model the OutputImage concept!");


  struct WithExtensionImage : Image, mln::experimental::Image<WithExtensionImage>
  {
    struct Extension
    {
    };

    using extension_type = Extension;

    using extension_category = mln::extension::custom_extension_tag;

    image_extension_t<WithExtensionImage> extension() const;
  };

  PYLENE_CONCEPT_TS_ASSERT(mln::concepts::WithExtensionImage<WithExtensionImage>,
                           "WithExtensionImage archetype does not model the WithExtensionImage concept!");

} // namespace mln::archetypes
