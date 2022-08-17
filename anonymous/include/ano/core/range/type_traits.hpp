#pragma once

#include <ano/core/range/private/mdcursor.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>


namespace ano::ranges
{

  template <class R>
  struct mdrange_row_type
  {
    using type = R;
  };

  template <MDRange R>
  struct mdrange_row_type<R>
  {
    using type = ::ranges::detail::cursor_value_t<::ranges::detail::begin_cursor_t<R>>;
  };

  namespace details
  {
    template <class R>
    struct mdrange_value_type
    {
      using type = ::ranges::range_value_t<R>;
    };

    template <MDRange R>
    struct mdrange_value_type<R>
    {
      using type =
          ::ranges::detail::cursor_value_t<::ranges::detail::begin_cursor_t<::ranges::detail::begin_cursor_t<R>>>;
    };

    template <class R>
    struct mdrange_reference_type
    {
      using type = ::ranges::range_reference_t<R>;
    };

    template <MDRange R>
    struct mdrange_reference_type<R>
    {
      using type =
          ::ranges::detail::cursor_reference_t<::ranges::detail::begin_cursor_t<::ranges::detail::begin_cursor_t<R>>>;
    };

  } // namespace details


  template <class R>
  using mdrange_row_t = typename mdrange_row_type<R>::type;

  template <class R>
  using mdrange_value_t = typename details::mdrange_value_type<R>::type;

  template <class R>
  using mdrange_reference_t = typename details::mdrange_reference_type<R>::type;
} // namespace ano::ranges
