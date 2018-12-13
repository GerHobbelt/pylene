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
      typename image_ch_value_t<Ima, mln::archetypes::Value>;
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
    stl::ConvertibleTo<pixel_point_t<image_pixel_t<Ima>>, image_point_t<Ima>> &&
    stl::ConvertibleTo<pixel_reference_t<image_pixel_t<Ima>>, image_reference_t<Ima>> &&
    // Here we don't want a convertible constraint as value_type is the decayed type and should really be the same
    stl::Same<pixel_value_t<image_pixel_t<Ima>>, image_value_t<Ima>> && 
    stl::CommonReference<image_reference_t<Ima>&&, image_value_t<Ima>&> &&
		stl::CommonReference<image_reference_t<Ima>&&, image_value_t<Ima>&&> &&
		stl::CommonReference<image_value_t<Ima>&&, const image_value_t<Ima>&> &&
    requires(Ima ima, const Ima cima, image_domain_t<Ima> d, image_point_t<Ima> p) {
      { cima.template ch_value<mln::archetypes::Value>() }
                            -> stl::ConvertibleTo<image_ch_value_t<Ima, mln::archetypes::Value>>&&; // Image builder (FIXME: improve builder design)
      { cima.concretize() } -> stl::ConvertibleTo<image_concrete_t<Ima>>&&;                         // Image builder (FIXME: improve builder design)
      { cima.domain() }     -> image_domain_t<Ima>;
      { ima.new_pixels() }  -> stl::ForwardRange&&;
      { ima.new_values() }  -> stl::ForwardRange&&;

      requires detail::RangeValueTypeConvertibleTo<decltype(ima.new_pixels()), image_pixel_t<Ima>>;
      requires detail::RangeValueTypeConvertibleTo<decltype(ima.new_values()), image_value_t<Ima>>;
    };


  namespace detail
  {

    // WritableImage
    template<typename WIma>
    concept WritableImage =
      Image<WIma> &&
      OutputPixel<image_pixel_t<WIma>> &&
      requires(WIma ima) {
        { ima.new_values() }  -> stl::OutputRange<image_value_t<WIma>>&&;
        // Check Writability of each pixel of the range
        requires OutputPixel<
                   std::common_type_t<
                     stl::iter_value_t<stl::iterator_t<decltype(ima.new_pixels())>>,
                     image_pixel_t<WIma>>>;
      };

  } // namespace detail


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
    requires (Ima ima, const Ima cima, image_index_t<Ima> k, image_point_t<Ima> p) {
      { ima[k] }                  -> image_reference_t<Ima>; // For concrete image it returns a const_reference
      { cima.point_at_index(k) }  -> image_point_t<Ima>;
      { cima.index_of_point(p) }  -> image_index_t<Ima>;
      { cima.delta_index(p) }     -> image_index_t<Ima>;
    };


  namespace detail
  {

    // WritableIndexableImage
    template<typename WIma>
    concept WritableIndexableImage =
      WritableImage<WIma> &&
      IndexableImage<WIma> &&
      requires(WIma ima, image_index_t<WIma> k, image_value_t<WIma> v) {
        { ima[k] = v } -> image_reference_t<WIma>;
      };
    
  } // namespace detail


  // AccessibleImage
  template <typename Ima>
  concept AccessibleImage =
    Image<Ima> &&
    image_accessible_v<Ima> &&
    requires (Ima ima, image_point_t<Ima> p) {
      { ima(p) }              -> image_reference_t<Ima>; // For concrete image it returns a const_reference
      { ima.at(p) }           -> image_reference_t<Ima>; // idem
      { ima.new_pixel(p) }    -> image_pixel_t<Ima>; // For concrete image pixel may propagate constness
      { ima.new_pixel_at(p) } -> image_pixel_t<Ima>; // idem
    };


  namespace detail
  {

    // WritableAccessibleImage
    template<typename WIma>
    concept WritableAccessibleImage =
      detail::WritableImage<WIma> &&
      AccessibleImage<WIma> &&
      requires(WIma ima, image_point_t<WIma> p, image_value_t<WIma> v) {
        { ima(p) = v }    -> image_reference_t<WIma>;
        { ima.at(p) = v } -> image_reference_t<WIma>;
      };
  
  } // namespace detail


  // BidirectionalImage (not in STL term)
  template <typename Ima>
  concept BidirectionalImage =
    Image<Ima> &&
    stl::DerivedFrom<image_category_t<Ima>, bidirectional_image_tag> &&
    requires (Ima ima) {
      { ima.new_pixels() }  -> ReversibleRange&&;
      { ima.new_values() }  -> ReversibleRange&&;
    };


  namespace detail
  {

    // WritableBidirectionalImage
    template<typename WIma>
    concept WritableBidirectionalImage =
      WritableImage<WIma> &&
      BidirectionalImage<WIma>;

  } // namespace detail


  // RawImage (not contiguous, stride = padding)
  template <typename Ima>
  concept RawImage =
    IndexableImage<Ima> &&
    AccessibleImage<Ima> &&
    BidirectionalImage<Ima> &&
    stl::DerivedFrom<image_category_t<Ima>, raw_image_tag> &&
    requires (Ima ima, const Ima cima, int dim) {
      { ima.data() }        -> stl::ConvertibleTo<const image_value_t<Ima>*>&&; // data() may be proxied by a view
      { cima.strides(dim) } -> std::ptrdiff_t;
    };


  namespace detail
  {

    // WritableRawImage
    template<typename WIma>
    concept WritableRawImage =
      WritableImage<WIma> &&
      WritableIndexableImage<WIma> &&
      WritableAccessibleImage<WIma> &&
      WritableBidirectionalImage<WIma> &&
      RawImage<WIma> &&
      requires(WIma ima, image_value_t<WIma> v) {
        { ima.data() }        -> stl::ConvertibleTo<image_value_t<WIma>*>&&;
        { *(ima.data()) = v };
      };
  
  } // namespace detail


  // OutputImage
  // Usage: RawImage<I> && OutputImage<I>
  template <typename Ima>
  concept OutputImage =
    (not IndexableImage<Ima> && not AccessibleImage<Ima> && not BidirectionalImage<Ima> && not RawImage<Ima> &&
      detail::WritableImage<Ima>) ||
    (not AccessibleImage<Ima> && not BidirectionalImage<Ima> && not RawImage<Ima> &&
      detail::WritableIndexableImage<Ima>) ||
    (not IndexableImage<Ima> && not BidirectionalImage<Ima> && not RawImage<Ima> &&
      detail::WritableAccessibleImage<Ima>) ||
    (not IndexableImage<Ima> && not AccessibleImage<Ima> && not RawImage<Ima> &&
      detail::WritableBidirectionalImage<Ima> ) ||
    (detail::WritableRawImage<Ima>);


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


// TODO: think about ViewImage vs. ConcreteImage

#endif // PYLENE_CONCEPT_TS_ENABLED

  // clang-format on

} // namespace mln::concepts

// Validate concept
#include <mln/core/concept/new/archetype/image.hpp>
