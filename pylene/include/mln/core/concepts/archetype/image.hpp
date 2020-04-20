#pragma once


#include <mln/core/concepts/archetype/domain.hpp>
#include <mln/core/concepts/archetype/pixel.hpp>
#include <mln/core/concepts/image.hpp>

#include <mln/core/private/traits/image.hpp>
#include <mln/core/private/traits/pixel.hpp>
#include <range/v3/range/traits.hpp>
#include <type_traits>


namespace mln::archetypes
{

  // This file defines the following archetypes
  // Image
  // InputImage
  // OutputImage
  // (Output)AccessibleImage
  // (Output)IndexableImage
  // (Output)IndexableAndAccessibleImage


  namespace details
  {
    template <class I>
    struct AsImage : I, mln::details::Image<AsImage<I>>
    {
      using I::I;

      using concrete_type = AsImage<typename I::concrete_type>;
      concrete_type concretize() const;


      template <typename V>
      using ch_value_type = AsImage<typename I::template ch_value_type<V>>;

      template <typename V>
      ch_value_type<V> ch_value() const;
    };


    struct ConcreteImage
    {
      using new_pixel_type = archetypes::Pixel;
      using value_type     = pixel_value_t<mln::archetypes::Pixel>;
      using reference      = pixel_reference_t<mln::archetypes::Pixel>;
      using point_type     = ::ranges::range_value_t<Domain>;
      using domain_type    = Domain;
      using category_type  = forward_image_tag;
      using concrete_type  = ConcreteImage;

      template <class V>
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

      domain_type domain() const;


      struct pixel_range
      {
        const new_pixel_type* begin();
        const new_pixel_type* end();
      };
      pixel_range pixels();


      struct value_range
      {
        const value_type* begin();
        const value_type* end();
      };

      value_range values();
    };


    struct ViewImage : ConcreteImage
    {
      using view = std::true_type;

      ViewImage()                 = delete;
      ViewImage(const ViewImage&) = default;
      ViewImage(ViewImage&&)      = default;
      ViewImage& operator=(const ViewImage&) = delete;
      ViewImage& operator=(ViewImage&&) = delete;
    };

    using Image = ViewImage;


    struct OutputImage : Image
    {
      using new_pixel_type = archetypes::OutputPixel;
      using reference      = pixel_reference_t<mln::archetypes::OutputPixel>;

      struct pixel_range
      {
        const new_pixel_type* begin();
        const new_pixel_type* end();
      };

      pixel_range pixels();

      struct value_range
      {
        value_type* begin();
        value_type* end();
      };

      value_range values();
    };


    struct OutputIndexableImage : OutputImage
    {
      using index_type = int;
      using indexable  = std::true_type;

      using concrete_type = OutputIndexableImage;

      template <class V>
      using ch_value_type = OutputIndexableImage;

      reference operator[](index_type);
    };


    struct IndexableImage : Image
    {
      using index_type = int;
      using indexable  = std::true_type;

      using concrete_type = OutputIndexableImage;

      template <class V>
      using ch_value_type = OutputIndexableImage;


      reference operator[](index_type);
    };

    struct OutputAccessibleImage : OutputImage
    {
      using accessible    = std::true_type;
      using concrete_type = OutputAccessibleImage;

      template <class V>
      using ch_value_type = OutputAccessibleImage;


      reference      operator()(point_type);
      reference      at(point_type);
      new_pixel_type new_pixel(point_type);
      new_pixel_type new_pixel_at(point_type);
    };


    struct AccessibleImage : Image
    {
      using accessible    = std::true_type;
      using concrete_type = OutputAccessibleImage;

      template <class V>
      using ch_value_type = OutputAccessibleImage;

      reference      operator()(point_type);
      reference      at(point_type);
      new_pixel_type new_pixel(point_type);
      new_pixel_type new_pixel_at(point_type);
    };


    struct OutputIndexableAndAccessibleImage : OutputAccessibleImage
    {
      using index_type = int;
      using indexable  = std::true_type;

      using concrete_type = OutputIndexableAndAccessibleImage;

      template <class V>
      using ch_value_type = OutputIndexableAndAccessibleImage;


      reference  operator[](index_type);
      point_type point_at_index(index_type) const;
      index_type index_of_point(point_type) const;
      index_type delta_index(point_type) const;
    };


    struct IndexableAndAccessibleImage : AccessibleImage
    {
      using index_type = int;
      using indexable  = std::true_type;

      using concrete_type = OutputIndexableAndAccessibleImage;

      template <class V>
      using ch_value_type = OutputIndexableAndAccessibleImage;

      reference  operator[](index_type);
      point_type point_at_index(index_type) const;
      index_type index_of_point(point_type) const;
      index_type delta_index(point_type) const;
    };


    struct BidirectionalImage : Image
    {
      using category_type = bidirectional_image_tag;

      struct pixel_range
      {
        const new_pixel_type* begin();
        const new_pixel_type* end();
        pixel_range           reversed();
      };

      pixel_range pixels();

      struct value_range
      {
        const value_type* begin();
        const value_type* end();
        value_range       reversed();
      };

      value_range values();
    };


    struct OutputBidirectionalImage : BidirectionalImage
    {
      using new_pixel_type = archetypes::OutputPixel;
      using reference      = pixel_reference_t<mln::archetypes::OutputPixel>;

      struct value_range
      {
        value_type* begin();
        value_type* end();
        value_range reversed();
      };
      value_range values();


      struct pixel_range
      {
        const new_pixel_type* begin();
        const new_pixel_type* end();
        pixel_range           reversed();
      };

      pixel_range pixels();
    };


    struct RawImage : IndexableAndAccessibleImage
    {
      using category_type = raw_image_tag;
      using pixel_range   = BidirectionalImage::pixel_range;
      using value_range   = BidirectionalImage::value_range;

      pixel_range pixels();
      value_range values();


      const value_type* data() const;
      std::ptrdiff_t    strides(int) const;
    };


    struct OutputRawImage : OutputIndexableAndAccessibleImage
    {
      using category_type = raw_image_tag;
      using pixel_range   = OutputBidirectionalImage::pixel_range;
      using value_range   = OutputBidirectionalImage::value_range;

      pixel_range pixels();
      value_range values();

      value_type*    data() const;
      std::ptrdiff_t strides(int) const;
    };


    struct WithExtensionImage : Image
    {
      struct Extension
      {
      };

      using extension_type = Extension;

      using extension_category = mln::extension::custom_extension_tag;

      extension_type extension() const;
    };

  } // namespace details


  using Image         = details::AsImage<details::Image>;
  using ConcreteImage = details::AsImage<details::ConcreteImage>;
  using ViewImage     = details::AsImage<details::ViewImage>;

  using ForwardImage       = Image;
  using BidirectionalImage = details::AsImage<details::BidirectionalImage>;
  using RawImage           = details::AsImage<details::RawImage>;

  using InputImage                  = Image;
  using IndexableImage              = details::AsImage<details::IndexableImage>;
  using AccessibleImage             = details::AsImage<details::AccessibleImage>;
  using IndexableAndAccessibleImage = details::AsImage<details::IndexableAndAccessibleImage>;


  using OutputImage              = details::AsImage<details::OutputImage>;
  using OutputForwardImage       = OutputImage;
  using OutputBidirectionalImage = details::AsImage<details::OutputBidirectionalImage>;
  using OutputRawImage           = details::AsImage<details::OutputRawImage>;

  using OutputIndexableImage              = details::AsImage<details::OutputIndexableImage>;
  using OutputAccessibleImage             = details::AsImage<details::OutputAccessibleImage>;
  using OutputIndexableAndAccessibleImage = details::AsImage<details::OutputIndexableAndAccessibleImage>;

  using WithExtensionImage = details::AsImage<details::WithExtensionImage>;
} // namespace mln::archetypes
