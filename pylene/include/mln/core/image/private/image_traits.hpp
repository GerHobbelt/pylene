#pragma once

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


  // Extension trait
  template <class I>
  using image_extension_category_t = typename I::extension_category;
  template <class I>
  using image_extension_t = typename I::extension_type;


  // Index trait
  template <class I>
  using image_indexable_t = typename I::indexable;
  template <class I>
  inline constexpr auto image_indexable_v = image_indexable_t<I>::value;
  template <class I>
  using image_index_t = typename I::index_type;


  // Accessible trait
  template <class I>
  using image_accessible_t = typename I::accessible;
  template <class I>
  inline constexpr auto image_accessible_v = image_accessible_t<I>::value;


  // Change value type trait
  template <class I, class V>
  using image_ch_value_t = typename I::template ch_value_type<V>;

  // Category trait
  template <class I>
  using image_category_t = typename I::category_type;

  // View trait
  template <class I>
  using image_view_t = typename I::view;
  template <class I>
  inline constexpr auto image_view_v = image_view_t<I>::value;
} // namespace mln
