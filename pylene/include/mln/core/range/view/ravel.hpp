#pragma once

#include <range/v3/view/facade.hpp>
#include <concepts/type_traits.hpp>




namespace mln::ranges
{

  namespace details
  {

    template <class Cur>
    requires MDCursor<Cur>
    struct ravel_cursor
    {
      using I = ::ranges::detail::begin_cursor_t<Cur>;
      using S = ::ranges::detail::end_cursor_t<Cur>;

      decltype(auto) read() const { return ::ranges::range_access::read(m_inner_cursor); }

      bool equal(const ravel_cursor& other) const
      {
        return ::ranges::range_access::equal(m_outer_cursor, other.m_outer_cursor) &&
               ::ranges::range_access::equal(m_inner_cursor, other.m_inner_cursor);
      }

      bool equal(::ranges::default_sentinel_t) const
      {
        return ::ranges::range_access::equal(m_outer_cursor, ::ranges::default_sentinel);
      }

      void next()
      {
        ::ranges::range_access::next(m_inner_cursor);
        if (::ranges::range_access::equal(m_inner_cursor, m_inner_end_cursor))
        {
          ::ranges::range_access::next(m_outer_cursor);
          if (!::ranges::range_access::equal(m_outer_cursor, ::ranges::default_sentinel))
          {
            m_inner_cursor     = m_outer_cursor.begin_cursor();
            m_inner_end_cursor = m_outer_cursor.end_cursor();
          }
        }
      }

      ravel_cursor() = default;
      ravel_cursor(Cur cur)
        : m_outer_cursor{cur}
        , m_inner_cursor{m_outer_cursor.begin_cursor()}
        , m_inner_end_cursor{m_outer_cursor.end_cursor()}
      {
      }

      Cur m_outer_cursor;
      I   m_inner_cursor;
      S   m_inner_end_cursor;
    };
  } // namespace details

  template <class Rng>
  requires MDRange<Rng>
  struct ravel_view : ::ranges::view_facade<ravel_view<Rng>>
  {
    using cursor = details::ravel_cursor<::ranges::detail::begin_cursor_t<Rng>>;

    cursor begin_cursor() const { return cursor(m_rng.begin_cursor()); }
    ::ranges::default_sentinel_t end_cursor() const { return {}; }

    ravel_view() = default;
    ravel_view(Rng rng)
      : m_rng{std::move(rng)}
    {
    }

    Rng m_rng;
  };

  namespace view
  {

    template <MDRange R>
    auto ravel(R rng)
    {
      return ravel_view<R>{std::move(rng)};
    }

    template <::ranges::cpp20::range R>
    decltype(auto) ravel(R&& rng)
    {
      return std::forward<R>(rng);
    }

  }
}
