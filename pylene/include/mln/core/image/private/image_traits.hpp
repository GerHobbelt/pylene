#pragma once

namespace mln
{

  template <class I, class V>
  using ch_value_t = typename I::template ch_value_type<V>;

  template <class I>
  using concrete_t = typename I::concrete_type;


  template <class I>
  using image_pixel_t = typename I::new_pixel_type;

  template <class I>
  using image_reference_t = typename I::reference;

  template <class I>
  using image_value_t = typename I::value_type;

  template <class I>
  using image_point_t = typename I::point_type;

}
