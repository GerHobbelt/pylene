#pragma once

#include <type_traits>


namespace mln::ranges
{

  template <class R, class = void>
  struct has_reverse_method;

  template <class R>
  constexpr bool has_reverse_method_v = has_reverse_method<R>::value;


  template <class R, class = void>
  struct has_rows_method;

  template <class R>
  constexpr bool has_rows_method_v = has_rows_method<R>::value;

  template <class R, class>
  struct has_reverse_method : std::false_type
  {
  };


  template <class R>
  struct has_reverse_method<R, std::void_t<decltype(std::declval<R>().reversed())>> : std::true_type
  {
  };

  template <class T>
  using is_reversible_range = has_reverse_method<T>;

  template <class T>
  inline constexpr bool is_reversible_range_v = is_reversible_range<T>::value;


  template <class R, class>
  struct has_rows_method : std::false_type
  {
  };

  template <class R>
  struct has_rows_method<R, std::void_t<decltype(std::declval<R>().rows())>> : std::true_type
  {
  };

  template <class T>
  using is_segmented_range = has_rows_method<T>;

  template <class T>
  inline constexpr bool is_segmented_range_v = is_segmented_range<T>::value;

} // namespace mln::ranges
