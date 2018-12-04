#pragma once

namespace mln
{

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

  template <class I>
  using image_index_t = typename I::index_type;

  template <class I>
  using image_extension_t = typename I::extension_type;

  template <class I>
  using image_extension_category_t = typename I::extension_category;


  template <class I, class V>
  using image_changed_value_t = typename I::template changed_value_type<V>;

  template <class I>
  using image_indexable_t = typename I::indexable;
  template <class I>
  inline constexpr auto image_indexable_v = image_indexable_t<I>::value;

  template <class I>
  using image_accessible_t = typename I::accessible;

  template <class I>
  inline constexpr auto image_accessible_v = image_accessible_t<I>::value;

  template <class I>
  using image_reversible_t = typename I::reversible;
  template <class I>
  inline constexpr auto image_reversible_v = image_reversible_t<I>::value;

  template <class I>
  using image_random_access_t = typename I::random_access;
  template <class I>
  inline constexpr auto image_random_access_v = image_random_access_t<I>::value;

  template <class I>
  using image_changeable_value_t = typename I::changeable_value;
  template <class I>
  inline constexpr auto image_changeable_value_v = image_changeable_value_t<I>::value;


  template <class I>
  using image_writable_t = typename I::writable;
  template <class I>
  inline constexpr auto image_writable_v = image_writable_t<I>::value;

  template <class I>
  using image_raw_t = typename I::raw;
  template <class I>
  inline constexpr auto image_raw_v = image_raw_t<I>::value;
} // namespace mln