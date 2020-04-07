#pragma once

#include <mln/core/extension/extension_traits.hpp>

namespace mln
{
  // Core traits
  template <class I>
  using image_pixel_t = typename I::new_pixel_type;

  template <class I>
  using image_reference_t = typename I::reference;

  template <class I>
  using image_value_t = typename I::value_type;

  template <class I>
  using image_point_t = typename I::point_type;

  template <class I>
  using image_domain_t = typename I::domain_type;

  template <class I>
  using image_concrete_t = typename I::concrete_type;

  template <class I, class V>
  using image_ch_value_t = typename I::template ch_value_type<V>;


  // Extension trait
  template <class I>
  using image_extension_category_t = typename I::extension_category;
  template <class I>
  using image_extension_t = typename I::extension_type;

  template <class I>
  using image_has_extension = std::is_convertible<image_extension_category_t<I>, mln::extension::custom_extension_tag>;

  template <class I>
  using image_has_extension_t = typename image_has_extension<I>::type;

  template <class I>
  inline constexpr bool image_has_extension_v = image_has_extension<I>::value;

  template <class I>
  using image_has_border = std::is_convertible<image_extension_category_t<I>, mln::extension::border_extension_tag>;

  template <class I>
  using image_has_border_t = typename image_has_border<I>::type;

  template <class I>
  inline constexpr bool image_has_border_v = image_has_border<I>::value;


  // Index trait
  template <class I>
  using image_indexable_t = typename I::indexable;

  template <class I>
  inline constexpr bool image_indexable_v = image_indexable_t<I>::value;


  template <class I>
  using image_index_t = typename I::index_type;


  // Accessible trait
  template <class I>
  using image_accessible_t = typename I::accessible;
  template <class I>
  inline constexpr bool image_accessible_v = image_accessible_t<I>::value;


  // Category trait
  template <class I>
  using image_category_t = typename I::category_type;

  // View trait
  template <class I>
  using image_view_t = typename I::view;
  template <class I>
  inline constexpr auto image_view_v = image_view_t<I>::value;
} // namespace mln
