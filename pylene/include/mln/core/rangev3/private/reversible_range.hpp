#pragma once

#include <mln/core/rangev3/range_traits.hpp>

#include <type_traits>

namespace mln::ranges
{
  template <class T>
  using is_reversible_range = has_reverse_method<T>;

  template <class T>
  inline constexpr bool is_reversible_range_v = is_reversible_range<T>::value;

} // namespace mln::ranges
