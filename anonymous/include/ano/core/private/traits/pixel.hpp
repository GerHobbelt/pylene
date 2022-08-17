#pragma once

namespace ano
{

  template <class P>
  using pixel_value_t = typename P::value_type;

  template <class P>
  using pixel_reference_t = typename P::reference;

  template <class P>
  using pixel_point_t = typename P::point_type;

} // namespace ano
