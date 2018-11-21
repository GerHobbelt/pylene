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

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class R, class>
  struct has_reverse_method : std::false_type
  {
  };


  template <class R>
  struct has_reverse_method<R, std::void_t<decltype(std::declval<R>().reversed())>> : std::true_type
  {
  };

  template <class R, class>
  struct has_rows_method : std::false_type
  {
  };


  template <class R>
  struct has_rows_method<R, std::void_t<decltype(std::declval<R>().rows())>> : std::true_type
  {
  };

} // namespace mln::ranges
