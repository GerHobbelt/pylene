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
    stl::DerivedFrom<image_category_t<Ima>, new_forward_image_tag> &&
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
    stl::Same<image_point_t<Ima>, typename image_pixel_t<Ima>::point_type> &&
    stl::Same<image_value_t<Ima>, typename image_pixel_t<Ima>::value_type> &&
    stl::Same<image_reference_t<Ima>, typename image_pixel_t<Ima>::reference> &&
    stl::CommonReference<image_reference_t<Ima>&&, image_value_t<Ima>&> &&
		stl::CommonReference<image_reference_t<Ima>&&, image_value_t<Ima>&&> &&
		stl::CommonReference<image_value_t<Ima>&&, const image_value_t<Ima>&> &&
    requires(Ima ima, image_domain_t<Ima> d, image_point_t<Ima> p) {
      { ima.template ch_value<archetype::value_archetype>() }
                            -> stl::ConvertibleTo<image_ch_value_t<Ima, archetype::value_archetype>>&&; // Image builder (FIXME: improve builder design)
      { ima.concretize() }  -> stl::ConvertibleTo<image_concrete_t<Ima>>&&;                             // Image builder (FIXME: improve builder design)
      { ima.domain() }      -> image_domain_t<Ima>;
      { ima.new_pixels() }  -> stl::ForwardRange&&;
      { ima.new_values() }  -> stl::ForwardRange&&;

      requires RangeValueTypeConvertibleTo<decltype(ima.new_pixels()), image_pixel_t<Ima>>;
      requires RangeValueTypeConvertibleTo<decltype(ima.new_values()), image_value_t<Ima>>;
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
    requires (Ima ima, image_index_t<Ima> k, image_point_t<Ima> p) {
      { ima[k] }                -> image_reference_t<Ima>;
      { ima.point_of_index(k) } -> image_point_t<Ima>;
      { ima.index_at_point(p) } -> image_index_t<Ima>;
      { ima.delta_index(p) }    -> image_index_t<Ima>;
    };


  // AccessibleImage
  template <typename Ima>
  concept AccessibleImage =
    Image<Ima> &&
    image_accessible_v<Ima> &&
    requires (Ima ima, image_point_t<Ima> p) {
      { ima(p) }              -> image_reference_t<Ima>;
      { ima.at(p) }           -> image_reference_t<Ima>;
      { ima.new_pixel(p) }    -> image_pixel_t<Ima>;
      { ima.new_pixel_at(p) } -> image_pixel_t<Ima>;
    };


  // BidirectionalImage (not in STL term)
  template <typename Ima>
    concept BidirectionalImage =
      Image<Ima> &&
      stl::DerivedFrom<image_category_t<Ima>, new_bidirectional_image_tag> &&
      requires (Ima ima) {
        { ima.new_pixels() }  -> ReversibleRange&&;
        { ima.new_values() }  -> ReversibleRange&&;
      };


  // RawImage (not contiguous, stride = padding)
  template <typename Ima>
  concept RawImage =
    IndexableImage<Ima> &&
    AccessibleImage<Ima> &&
    BidirectionalImage<Ima> &&
    stl::DerivedFrom<image_category_t<Ima>, new_raw_image_tag> &&
    requires (Ima ima, const Ima cima, int dim) {
      { ima.data() }        -> image_value_t<Ima>*;
      { cima.strides(dim) } -> std::size_t;
    };


  // ExtendedImage
  template <typename Ima>
  concept ExtendedImage =
    Image<Ima> &&
    requires {
      typename image_extension_t<Ima>;
    } &&
    not stl::Same<mln::extension::none_extension_tag, image_extension_category_t<Ima>> &&
    requires (Ima ima, image_point_t<Ima> p) {
      { ima.extension() } -> image_extension_t<Ima>;
    };




/*
template <RawImage I>
OutputImage = WritableRawImage<I>;


(WritableIndexableImage) ||
(WritableRawImage)
 || ...
   
  RawImage && OutputImage
   */

   // TODO: writable
// TODO: think about ViewImage vs. ConcreteImage

#endif // PYLENE_CONCEPT_TS_ENABLED

  // clang-format on

} // namespace mln::concepts

// Validate concept
#include <mln/core/concept/new/archetype/image.hpp>
