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
  using image_difference_t = typename I::difference_type;

  template <class I>
  using image_index_t = typename I::index_type;

} // namespace mln