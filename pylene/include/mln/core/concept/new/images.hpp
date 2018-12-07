#pragma once

#include <mln/core/concept/new/cmcstl2.hpp>

#include <mln/core/concept/new/domains.hpp>
#include <mln/core/concept/new/indexes.hpp>
#include <mln/core/concept/new/pixels.hpp>
#include <mln/core/concept/new/points.hpp>
#include <mln/core/concept/new/ranges.hpp>
#include <mln/core/concept/new/values.hpp>

#include <mln/core/concept/new/archetype/value.hpp>

#include <mln/core/extension/extension_traits.hpp>
#include <mln/core/image/private/image_traits.hpp>
#include <mln/core/image/private/pixel_traits.hpp>
#include <mln/core/image_category.hpp>


#include <cstddef>
#include <type_traits>
#include <utility>

namespace mln::concepts
{

  // clang-format off

#ifdef PYLENE_CONCEPT_TS_ENABLED

  // Image
  template<typename Ima>
  concept Image =
    // FIXME : inheritance from New_Image required for the moment
    stl::Semiregular<Ima> &&
    stl::DerivedFrom<image_category_t<Ima>, forward_image_tag> &&
    requires {
      typename image_pixel_t<Ima>;
      typename image_point_t<Ima>;
      typename image_value_t<Ima>;
      typename image_domain_t<Ima>;
      typename image_reference_t<Ima>;
      typename image_concrete_t<Ima>;
      typename image_ch_value_t<Ima, archetype::value_archetype>;
      // traits
      typename image_indexable_t<Ima>;
      typename image_accessible_t<Ima>;
      typename image_extension_category_t<Ima>;
      typename image_category_t<Ima>;
    } &&
    Pixel<image_pixel_t<Ima>> &&
    Point<image_point_t<Ima>> &&
    Value<image_value_t<Ima>> &&
    Domain<image_domain_t<Ima>> &&
    stl::Same<image_point_t<Ima>, pixel_point_t<image_pixel_t<Ima>>> &&
    stl::Same<image_value_t<Ima>, pixel_value_t<image_pixel_t<Ima>>> &&
    stl::Same<image_reference_t<Ima>, pixel_reference_t<image_pixel_t<Ima>>> &&
    stl::CommonReference<image_reference_t<Ima>&&, image_value_t<Ima>&> &&
		stl::CommonReference<image_reference_t<Ima>&&, image_value_t<Ima>&&> &&
		stl::CommonReference<image_value_t<Ima>&&, const image_value_t<Ima>&> &&
    requires(Ima ima, const Ima cima, image_domain_t<Ima> d, image_point_t<Ima> p) {
      { cima.template ch_value<archetype::value_archetype>() }
                            -> stl::ConvertibleTo<image_ch_value_t<Ima, archetype::value_archetype>>&&; // Image builder (FIXME: improve builder design)
      { cima.concretize() } -> stl::ConvertibleTo<image_concrete_t<Ima>>&&;                             // Image builder (FIXME: improve builder design)
      { cima.domain() }     -> image_domain_t<Ima>;
      { ima.new_pixels() }  -> stl::ForwardRange&&;
      { ima.new_values() }  -> stl::ForwardRange&&;

      requires RangeValueTypeConvertibleTo<decltype(ima.new_pixels()), image_pixel_t<Ima>>;
      requires RangeValueTypeConvertibleTo<decltype(ima.new_values()), image_value_t<Ima>>;
    };


  // WritableImage
  template<typename WIma>
  concept WritableImage =
    Image<WIma> &&
    requires(WIma ima) {
      // FIXME : Writable Pixel
      // { ima.new_pixels() }  -> stl::OutputRange<image_value_t<WIma>>&&;
      { ima.new_values() }  -> stl::OutputRange<image_value_t<WIma>>&&;
    };


  // InputImage
  template <typename Ima>
  concept InputImage = Image<Ima>;


  // ForwardImage
  template <typename Ima>
  concept ForwardImage = InputImage<Ima>;


  // IndexableImage
  template <typename Ima>
  concept IndexableImage =
    Image<Ima> &&
    requires {
      typename image_index_t<Ima>;
    } &&
    image_indexable_v<Ima> &&
    requires (const Ima cima, image_index_t<Ima> k, image_point_t<Ima> p) {
      { cima[k] }                -> image_reference_t<Ima>;
      { cima.point_of_index(k) } -> image_point_t<Ima>;
      { cima.index_at_point(p) } -> image_index_t<Ima>;
      { cima.delta_index(p) }    -> image_index_t<Ima>;
    };


  // WritableIndexableImage
  template<typename WIma>
  concept WritableIndexableImage =
    WritableImage<WIma> &&
    IndexableImage<WIma> &&
    requires(WIma ima, image_index_t<WIma> k, image_value_t<WIma> v) {
      { ima[k] = v };
    };


  // AccessibleImage
  template <typename Ima>
  concept AccessibleImage =
    Image<Ima> &&
    image_accessible_v<Ima> &&
    requires (const Ima cima, image_point_t<Ima> p) {
      { cima(p) }              -> image_reference_t<Ima>;
      { cima.at(p) }           -> image_reference_t<Ima>;
      { cima.new_pixel(p) }    -> image_pixel_t<Ima>;
      { cima.new_pixel_at(p) } -> image_pixel_t<Ima>;
    };


  // WritableAccessibleImage
  template<typename WIma>
  concept WritableAccessibleImage =
    WritableImage<WIma> &&
    AccessibleImage<WIma> &&
    requires(WIma ima, image_point_t<WIma> p, image_value_t<WIma> v) {
      { ima(p) = v };
      { ima.at(p) = v };
    };


  // BidirectionalImage (not in STL term)
  template <typename Ima>
  concept BidirectionalImage =
    Image<Ima> &&
    stl::DerivedFrom<image_category_t<Ima>, bidirectional_image_tag> &&
    requires (Ima ima) {
      { ima.new_pixels() }  -> ReversibleRange&&;
      { ima.new_values() }  -> ReversibleRange&&;
    };


  // WritableBidirectionalImage
  template<typename WIma>
  concept WritableBidirectionalImage =
    WritableImage<WIma> &&
    BidirectionalImage<WIma>;


  // RawImage (not contiguous, stride = padding)
  template <typename Ima>
  concept RawImage =
    IndexableImage<Ima> &&
    AccessibleImage<Ima> &&
    BidirectionalImage<Ima> &&
    stl::DerivedFrom<image_category_t<Ima>, raw_image_tag> &&
    requires (Ima ima, const Ima cima, int dim) {
      { ima.data() }        -> stl::ConvertibleTo<const image_value_t<Ima>*>&&;
      { cima.strides(dim) } -> std::size_t;
    };


  // WritableRawImage
  template<typename WIma>
  concept WritableRawImage =
    WritableIndexableImage<WIma> &&
    WritableAccessibleImage<WIma> &&
    WritableBidirectionalImage<WIma> &&
    RawImage<WIma> &&
    requires(WIma ima, image_value_t<WIma> v) {
      { *(ima.data()) = v };
    };


  // ExtendedImage
  template <typename Ima>
  concept ExtendedImage =
    Image<Ima> &&
    requires {
      typename image_extension_t<Ima>;
    } &&
    not stl::Same<mln::extension::none_extension_tag, image_extension_category_t<Ima>> &&
    requires (const Ima cima, image_point_t<Ima> p) {
      { cima.extension() } -> image_extension_t<Ima>;
    };


  // OutputImage
  // Usage: RawImage<I> && OutputImage<I>
  template <typename Ima>
  concept OutputImage =
    (WritableImage<Ima>) ||               // alias for Image and ForwardImage
    (WritableIndexableImage<Ima>) ||
    (WritableAccessibleImage<Ima>) ||
    (WritableBidirectionalImage<Ima>) ||
    (WritableRawImage<Ima>);

// TODO: think about ViewImage vs. ConcreteImage

#endif // PYLENE_CONCEPT_TS_ENABLED

  // clang-format on

} // namespace mln::concepts

// Validate concept
#include <mln/core/concept/new/archetype/image.hpp>
