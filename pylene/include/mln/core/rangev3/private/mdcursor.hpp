#pragma once

#include <concepts/concepts.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/detail/range_access.hpp>
#include <range/v3/view/facade.hpp> // ::ranges::detail::begin_cursor_t

namespace mln::ranges
{

  template <class C>
  concept MDCursor =
    ::ranges::detail::forward_cursor<C> &&
    ::ranges::detail::forward_cursor<::ranges::detail::begin_cursor_t<C>> &&
    requires (C c)
    {
      { c.read() } -> ::ranges::cpp20::forward_range&&;
      c.end_cursor();
    };

  template <class C>
  concept NDCursor = ::concepts::semiregular<C> &&
    requires (C c)
  {
    { C::rank } -> ::concepts::same_as<int>;
    c.read();
    c.move_to_next(0);
    c.move_to_end(0);
  };

  template <class C>
  concept MDBidirectionalCursor = MDCursor<C> &&
    requires (C c)
  {
    c.move_to_prev();
    c.move_to_prev_line();
  };

  template <class R>
  concept MDRange =
    requires(R r)
  {
    { r.rows() } -> ::ranges::cpp20::forward_range&&;
    { r.begin_cursor() } -> MDCursor&&
    { r.end_cursor() } -> ::concepts::same_as<::ranges::default_sentinel_t>;
  };


  template <class R>
  concept MDBidirectionalRange = MDRange<R> && requires (R r)
  {
    { r.rrows() } -> ::ranges::cpp20::forward_range&&;
    { r.rbegin_cursor() } -> MDCursor&&;
    { r.rend_cursor() } -> ::concepts::same_as<::ranges::default_sentinel_t>;
  };

}
