#pragma once
#include "mdcursor.hpp"

#include <range/v3/iterator/basic_iterator.hpp>
#include <range/v3/view/subrange.hpp>
#include <range/v3/view/facade.hpp>

namespace mln::ranges::details
{




  template <class R>
  using rbegin_cursor_t = typename R::backward_cursor;

  template <class C>
  struct mdcursor_facade
  {
    auto read() const
    {
      using I  = ::ranges::detail::facade_iterator_t<C>;
      using S  = ::ranges::detail::facade_sentinel_t<C>;
      return ::ranges::subrange<I, S>{I{derived()->begin_cursor()}, S{derived()->end_cursor()}};
    }

  private:
    const C* derived() const { return static_cast<const C*>(this); }
  };


  template <class R>
  struct mdview_facade
  {
  public:
    auto rows() const
    {
      using C = ::ranges::detail::begin_cursor_t<R>;
      using I = ::ranges::basic_iterator<C>;
      using S = ::ranges::default_sentinel_t;
      C beg = static_cast<const R*>(this)->begin_cursor();
      S end;
      return ::ranges::subrange<I, S>{I{beg}, end};
    }

    template <class R2 = R>
    auto rrows() const requires( requires { typename R2::backward_cursor; } )
    {
      using C = rbegin_cursor_t<R>;
      using I = ::ranges::basic_iterator<C>;
      using S = ::ranges::default_sentinel_t;

      C beg = static_cast<const R*>(this)->rbegin_cursor();
      S end;
      return ::ranges::subrange<I, S>{I{beg}, end};
    }
  };

}
