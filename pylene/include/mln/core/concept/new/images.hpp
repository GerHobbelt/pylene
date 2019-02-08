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

namespace mln::experimental
{
  template <typename I>
  struct Image
  {
  };
} // namespace mln::experimental

namespace mln::concepts
{

  // clang-format off

#ifdef PYLENE_CONCEPT_TS_ENABLED

  // Image
  template <typename I>
  concept Image =
    // Minimum constraint on image object
    // Do not requires DefaultConstructible
    std::is_base_of_v<mln::experimental::Image<I>, I> &&
    stl::CopyConstructible<I> &&
    stl::MoveConstructible<I> &&
    stl::DerivedFrom<image_category_t<I>, forward_image_tag> &&
    requires {
      typename image_pixel_t<I>;
      typename image_point_t<I>;
      typename image_value_t<I>;
      typename image_domain_t<I>;
      typename image_reference_t<I>;
      typename image_concrete_t<I>;
      typename image_ch_value_t<I, mln::archetypes::Value>;
      // traits
      typename image_indexable_t<I>;
      typename image_accessible_t<I>;
      typename image_extension_category_t<I>;
      typename image_category_t<I>;
      typename image_view_t<I>;
    } &&
    Pixel<image_pixel_t<I>> &&
    Point<image_point_t<I>> &&
    Value<image_value_t<I>> &&
    Domain<image_domain_t<I>> &&
    stl::ConvertibleTo<pixel_point_t<image_pixel_t<I>>, image_point_t<I>> &&
    stl::ConvertibleTo<pixel_reference_t<image_pixel_t<I>>, image_reference_t<I>> &&
    // Here we don't want a convertible constraint as value_type is the decayed type and should really be the same
    stl::Same<pixel_value_t<image_pixel_t<I>>, image_value_t<I>> && 
    stl::CommonReference<image_reference_t<I>&&, image_value_t<I>&> &&
		stl::CommonReference<image_reference_t<I>&&, image_value_t<I>&&> &&
		stl::CommonReference<image_value_t<I>&&, const image_value_t<I>&> &&
    requires(I ima, const I cima, image_domain_t<I> d, image_point_t<I> p) {
      { cima.template ch_value<mln::archetypes::Value>() }
                            -> stl::ConvertibleTo<image_ch_value_t<I, mln::archetypes::Value>>&&; // Image builder (FIXME: improve builder design)
      { cima.concretize() } -> stl::ConvertibleTo<image_concrete_t<I>>&&;                         // Image builder (FIXME: improve builder design)
      { cima.domain() }     -> image_domain_t<I>;
      { ima.new_pixels() }  -> stl::ForwardRange&&;
      { ima.new_values() }  -> stl::ForwardRange&&;

      requires detail::RangeValueTypeConvertibleTo<decltype(ima.new_pixels()), image_pixel_t<I>>;
      requires detail::RangeValueTypeConvertibleTo<decltype(ima.new_values()), image_value_t<I>>;
    };


  namespace detail
  {

    // WritableImage
    template <typename I>
    concept WritableImage =
      Image<I> &&
      OutputPixel<image_pixel_t<I>> &&
      requires(I ima) {
        { ima.new_values() }  -> stl::OutputRange<image_value_t<I>>&&;
        // Check Writability of each pixel of the range
        requires OutputPixel<
                   std::common_type_t<
                     stl::iter_value_t<stl::iterator_t<decltype(ima.new_pixels())>>,
                     image_pixel_t<I>>>;
      };

  } // namespace detail


  // InputImage
  template <typename I>
  concept InputImage = Image<I>;


  // ForwardImage
  template <typename I>
  concept ForwardImage = InputImage<I>;


  // IndexableImage
  template <typename I>
  concept IndexableImage =
    Image<I> &&
    requires {
      typename image_index_t<I>;
    } &&
    image_indexable_v<I> &&
    requires (I ima, image_index_t<I> k) {
      { ima[k] }  -> image_reference_t<I>; // For concrete image it returns a const_reference
    };


  namespace detail
  {

    // WritableIndexableImage
    template <typename I>
    concept WritableIndexableImage =
      WritableImage<I> &&
      IndexableImage<I> &&
      requires(I ima, image_index_t<I> k, image_value_t<I> v) {
        { ima[k] = v } -> image_reference_t<I>;
      };
    
  } // namespace detail


  // AccessibleImage
  template <typename I>
  concept AccessibleImage =
    Image<I> &&
    image_accessible_v<I> &&
    requires (I ima, image_point_t<I> p) {
      { ima(p) }              -> image_reference_t<I>; // For concrete image it returns a const_reference
      { ima.at(p) }           -> image_reference_t<I>; // idem
      { ima.new_pixel(p) }    -> image_pixel_t<I>; // For concrete image pixel may propagate constness
      { ima.new_pixel_at(p) } -> image_pixel_t<I>; // idem
    };


  namespace detail
  {

    // WritableAccessibleImage
    template <typename I>
    concept WritableAccessibleImage =
      detail::WritableImage<I> &&
      AccessibleImage<I> &&
      requires(I ima, image_point_t<I> p, image_value_t<I> v) {
        { ima(p) = v }    -> image_reference_t<I>;
        { ima.at(p) = v } -> image_reference_t<I>;
      };
  
  } // namespace detail


  // IndexableAndAccessibleImage
  template <typename I>
  concept IndexableAndAccessibleImage =
    IndexableImage<I> &&
    AccessibleImage<I> &&
    requires (const I cima, image_index_t<I> k, image_point_t<I> p) {
      { cima.point_at_index(k) }  -> image_point_t<I>;
      { cima.index_of_point(p) }  -> image_index_t<I>;
      { cima.delta_index(p) }     -> image_index_t<I>;
    };


  namespace detail
  {

    // WritableIndexableAndAccessibleImage
    template <typename I>
    concept WritableIndexableAndAccessibleImage =
      IndexableAndAccessibleImage<I> &&
      detail::WritableImage<I> &&
      detail::WritableIndexableImage<I>;
    
  } // namespace detail


  // BidirectionalImage (not in STL term)
  template <typename I>
  concept BidirectionalImage =
    Image<I> &&
    stl::DerivedFrom<image_category_t<I>, bidirectional_image_tag> &&
    requires (I ima) {
      { ima.new_pixels() }  -> ReversibleRange&&;
      { ima.new_values() }  -> ReversibleRange&&;
    };


  namespace detail
  {

    // WritableBidirectionalImage
    template <typename I>
    concept WritableBidirectionalImage =
      WritableImage<I> &&
      BidirectionalImage<I>;

  } // namespace detail


  // RawImage (not contiguous, stride = padding)
  template <typename I>
  concept RawImage =
    IndexableAndAccessibleImage<I> &&
    BidirectionalImage<I> &&
    stl::DerivedFrom<image_category_t<I>, raw_image_tag> &&
    requires (I ima, const I cima, int dim) {
      { ima.data() }        -> stl::ConvertibleTo<const image_value_t<I>*>&&; // data() may be proxied by a view
      { cima.strides(dim) } -> std::ptrdiff_t;
    };


  namespace detail
  {

    // WritableRawImage
    template <typename I>
    concept WritableRawImage =
      WritableImage<I> &&
      WritableIndexableAndAccessibleImage<I> &&
      WritableBidirectionalImage<I> &&
      RawImage<I> &&
      requires(I ima, image_value_t<I> v) {
        { ima.data() }        -> stl::ConvertibleTo<image_value_t<I>*>&&;
        { *(ima.data()) = v };
      };

  } // namespace detail


  // OutputImage
  // Usage: RawImage<I> && OutputImage<I>
  template <typename I>
  concept OutputImage =
    (not ForwardImage<I> || (ForwardImage<I> && detail::WritableImage<I>)) &&
    (not IndexableImage<I> || (IndexableImage<I> && detail::WritableIndexableImage<I>)) &&
    (not AccessibleImage<I> || (AccessibleImage<I> && detail::WritableAccessibleImage<I>)) &&
    (not IndexableAndAccessibleImage<I> ||
      (IndexableAndAccessibleImage<I> && detail::WritableIndexableAndAccessibleImage<I>)) &&
    (not BidirectionalImage<I> || (BidirectionalImage<I> && detail::WritableBidirectionalImage<I>)) &&
    (not RawImage<I> || (RawImage<I> && detail::WritableRawImage<I>));


  // WithExtensionImage
  template <typename I>
  concept WithExtensionImage =
    Image<I> &&
    requires {
      typename image_extension_t<I>;
    } &&
    not stl::Same<mln::extension::none_extension_tag, image_extension_category_t<I>> &&
    requires (const I cima, image_point_t<I> p) {
      { cima.extension() } -> image_extension_t<I>;
    };


  // ConcreteImage
  template <typename I>
  concept ConcreteImage =
    Image<I> &&
    stl::Semiregular<I> &&  // A concrete image is default constructible
    not image_view_v<I>;
  

  // ViewImage
  template <typename I>
  concept ViewImage =
    Image<I> &&
    image_view_v<I>;

#endif // PYLENE_CONCEPT_TS_ENABLED

  // clang-format on

} // namespace mln::concepts

// Validate concept
#include <mln/core/concept/new/archetype/image.hpp>
