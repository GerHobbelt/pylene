#pragma once

#include <mln/core/concepts/range.hpp>
#include <mln/core/concepts/domain.hpp>
#include <mln/core/concepts/pixel.hpp>
#include <mln/core/concepts/point.hpp>

#include <mln/core/concepts/archetype/value.hpp>

#include <mln/core/private/traits/extension.hpp>
#include <mln/core/private/traits/image.hpp>
#include <mln/core/private/traits/pixel.hpp>



#include <concepts/concepts.hpp>
#include <concepts/type_traits.hpp>
#include <range/v3/range/concepts.hpp>

#include <mln/core/concepts/object.hpp> // mln::is_a

#include <cstddef>
#include <type_traits>
#include <utility>

namespace mln::details
{
  template <typename I>
  struct Image
  {
  };
} // namespace mln

namespace mln::concepts
{

  // clang-format off
  template <typename I>
  concept Image =
    // Minimum constraint on image object
    // Do not requires DefaultConstructible
    std::is_base_of_v<mln::details::Image<I>, I> &&
    ::concepts::copy_constructible<I> &&
    ::concepts::move_constructible<I> &&
    ::concepts::derived_from<image_category_t<I>, forward_image_tag> &&
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
    ::concepts::convertible_to<pixel_point_t<image_pixel_t<I>>, image_point_t<I>> &&
    ::concepts::convertible_to<pixel_reference_t<image_pixel_t<I>>, image_reference_t<I>> &&
    // Here we don't want a convertible constraint as value_type is the decayed type and should really be the same
    ::concepts::same_as<pixel_value_t<image_pixel_t<I>>, image_value_t<I>> &&
    ::concepts::common_reference_with<image_reference_t<I>&&, image_value_t<I>&> &&
    ::concepts::common_reference_with<image_reference_t<I>&&, image_value_t<I>&&> &&
    ::concepts::common_reference_with<image_value_t<I>&&, const image_value_t<I>&> &&
    requires(I ima, const I cima, image_domain_t<I> d, image_point_t<I> p) {
      { cima.template ch_value<mln::archetypes::Value>() }
      -> ::concepts::convertible_to<image_ch_value_t<I, mln::archetypes::Value>>;
      { cima.concretize() } -> ::concepts::convertible_to<image_concrete_t<I>>;
      { cima.domain() }     -> ::concepts::convertible_to<image_domain_t<I>>;
      { ima.pixels() }  -> mln::ranges::mdrange;
      { ima.values() }  -> mln::ranges::mdrange;
      requires ::concepts::convertible_to<mln::ranges::mdrange_value_t<decltype(ima.pixels())>, image_pixel_t<I>>;
      requires ::concepts::convertible_to<mln::ranges::mdrange_value_t<decltype(ima.values())>, image_value_t<I>>;
    };


  namespace detail
  {

    // WritableImage
    template <typename I>
    concept WritableImage =
      Image<I> &&
      OutputPixel<image_pixel_t<I>> &&
      requires(I ima) {
      { ima.values() }  -> mln::ranges::output_mdrange<image_value_t<I>>;
        // Check Writability of each pixel of the range
        requires OutputPixel<
                   std::common_type_t<
                     mln::ranges::mdrange_value_t<decltype(ima.pixels())>,
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
#if __GNUC__ == 9
    { ima[k] }  -> ::concepts::same_as<image_reference_t<I>>&&; // For concrete image it returns a const_reference
#else
    { ima[k] }  -> ::concepts::same_as<image_reference_t<I>>; // For concrete image it returns a const_reference
#endif
    };


  namespace detail
  {

    // WritableIndexableImage
    template <typename I>
    concept WritableIndexableImage =
      WritableImage<I> &&
      IndexableImage<I> &&
      requires(I ima, image_index_t<I> k, image_value_t<I> v) {
#if __GNUC__ == 9
        { ima[k] = v } -> ::concepts::same_as<image_reference_t<I>>&&;
#else
        { ima[k] = v } -> ::concepts::same_as<image_reference_t<I>>;
#endif
      };

  } // namespace detail


  // AccessibleImage
  template <typename I>
  concept AccessibleImage =
    Image<I> &&
    image_accessible_v<I> &&
    requires (I ima, image_point_t<I> p) {
#if __GNUC__ == 9
      { ima(p) }              -> ::concepts::same_as<image_reference_t<I>>&&; // For concrete image it returns a const_reference
      { ima.at(p) }           -> ::concepts::same_as<image_reference_t<I>>&&; // idem
      { ima.pixel(p) }    -> ::concepts::same_as<image_pixel_t<I>>&&; // For concrete image pixel may propagate constness
      { ima.pixel_at(p) } -> ::concepts::same_as<image_pixel_t<I>>&&; // idem
#else
      { ima(p) }              -> ::concepts::same_as<image_reference_t<I>>; // For concrete image it returns a const_reference
      { ima.at(p) }           -> ::concepts::same_as<image_reference_t<I>>; // idem
      { ima.pixel(p) }    -> ::concepts::same_as<image_pixel_t<I>>; // For concrete image pixel may propagate constness
      { ima.pixel_at(p) } -> ::concepts::same_as<image_pixel_t<I>>; // idem
#endif
    };


  namespace detail
  {

    // WritableAccessibleImage
    template <typename I>
    concept WritableAccessibleImage =
      detail::WritableImage<I> &&
      AccessibleImage<I> &&
      requires(I ima, image_point_t<I> p, image_value_t<I> v) {
        { ima(p) = v };
        { ima.at(p) = v };
      };

  } // namespace detail


  // IndexableAndAccessibleImage
  template <typename I>
  concept IndexableAndAccessibleImage =
    IndexableImage<I> &&
    AccessibleImage<I> &&
    requires (const I cima, image_index_t<I> k, image_point_t<I> p) {
      { cima.point_at_index(k) }  -> ::concepts::same_as<image_point_t<I>>;
      { cima.index_of_point(p) }  -> ::concepts::same_as<image_index_t<I>>;
      { cima.delta_index(p) }     -> ::concepts::same_as<image_index_t<I>>;
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
    ::concepts::derived_from<image_category_t<I>, bidirectional_image_tag> &&
    requires (I ima) {
    { ima.pixels() }  -> mln::ranges::reversible_mdrange;
    { ima.values() }  -> mln::ranges::reversible_mdrange;
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
    ::concepts::derived_from<image_category_t<I>, raw_image_tag> &&
    requires (I ima, const I cima, int dim) {
      { ima.data() }        -> ::concepts::convertible_to<const image_value_t<I>*>; // data() may be proxied by a view
      { cima.stride(dim) } -> ::concepts::same_as<std::ptrdiff_t>;
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
        { ima.data() }        -> ::concepts::convertible_to<image_value_t<I>*>;
        { *(ima.data()) = v };
      };

  } // namespace detail


  // OutputImage
  // Usage: RawImage<I> && OutputImage<I>
  template <typename I>
  concept OutputImage =
    (not ForwardImage<I> || (detail::WritableImage<I>)) &&
    (not IndexableImage<I> || (detail::WritableIndexableImage<I>)) &&
    (not AccessibleImage<I> || (detail::WritableAccessibleImage<I>)) &&
    (not IndexableAndAccessibleImage<I> ||
     (detail::WritableIndexableAndAccessibleImage<I>)) &&
    (not BidirectionalImage<I> || (detail::WritableBidirectionalImage<I>)) &&
    (not RawImage<I> || (detail::WritableRawImage<I>));


  // WithExtensionImage
  template <typename I>
  concept WithExtensionImage =
    Image<I> &&
    requires {
      typename image_extension_t<I>;
    } &&
    not ::concepts::same_as<mln::extension::none_extension_tag, image_extension_category_t<I>> &&
    requires (I ima, image_point_t<I> p) {
      { ima.extension() } -> ::concepts::same_as<image_extension_t<I>>;
    };


  // ConcreteImage
  template <typename I>
  concept ConcreteImage =
    Image<I> &&
    ::concepts::semiregular<I> &&  // A concrete image is default constructible
    not image_view_v<I>;

  // ViewImage
  template <typename I>
  concept ViewImage =
    Image<I> &&
    image_view_v<I>;



  // clang-format on

} // namespace mln::concepts
