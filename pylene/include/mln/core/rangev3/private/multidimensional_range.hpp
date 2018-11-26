#pragma once

#include <mln/core/rangev3/range_traits.hpp>

#include <type_traits>


namespace mln::ranges
{

  // Every class that inherits from this, is a segmented range
  struct multidimensional_range_base
  {
  };


  template <class T>
  using is_multidimensional_range =
      std::conjunction<has_rows_method<T>, std::is_base_of<multidimensional_range_base, T>>;

  template <class T>
  inline constexpr bool is_multidimensional_range_v = is_multidimensional_range<T>::value;
} // namespace mln::ranges
