#pragma once

#include <mln/core/rangev3/range_traits.hpp>

#include <type_traits>


namespace mln::ranges
{
  template <class T>
  using is_multidimensional_range = has_rows_method<T>;

  template <class T>
  inline constexpr bool is_multidimensional_range_v = is_multidimensional_range<T>::value;
} // namespace mln::ranges
