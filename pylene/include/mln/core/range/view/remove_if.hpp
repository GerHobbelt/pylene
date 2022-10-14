#pragma once

#include <mln/core/range/private/mdrange_facade.hpp>

#include <range/v3/view/remove_if.hpp>

#include <functional>

namespace mln::ranges
{
  namespace details
  {
    template <class Cur, class Fun>
    requires MDCursor<Cur>
    struct remove_if_mdcursor : mdcursor_facade<remove_if_mdcursor<Cur, Fun>>
    {
      struct cursor
      {
        decltype(auto) read() const { return ::ranges::range_access::read(m_cur); }

        void           next()
        {
          do
          {
            m_cur.next();
          } while (!m_cur.equal(m_end) && m_fun(m_cur.read()));
        }

        bool equal(::ranges::default_sentinel_t) const { return ::ranges::range_access::equal(m_cur, m_end); }
        bool equal(const cursor& other) const { return ::ranges::range_access::equal(m_cur, other.m_cur); }

        ::ranges::detail::begin_cursor_t<Cur> m_cur;
        ::ranges::detail::end_cursor_t<Cur>   m_end;
        ::ranges::semiregular_box_t<Fun>      m_fun;
      };


      cursor begin_cursor() const
      {
        auto c = ::ranges::range_access::begin_cursor(m_cur);
        auto e = ::ranges::range_access::end_cursor(m_cur);
        for (; !::ranges::range_access::equal(c, e); c.next())
          if (!m_fun(c.read()))
            break;
        return {c, e, m_fun};
      }

      ::ranges::default_sentinel_t end_cursor() const { return {}; }

      void next() { return ::ranges::range_access::next(m_cur); }
      bool equal(const remove_if_mdcursor& other) const { return ::ranges::range_access::equal(m_cur, other.m_cur); }
      bool equal(::ranges::default_sentinel_t) const { return ::ranges::range_access::equal(m_cur, ::ranges::default_sentinel); }

      Cur                              m_cur;
      ::ranges::semiregular_box_t<Fun> m_fun;
    };
  } // namespace details

  template <class Rng, class Fun>
  requires MDRange<Rng>
  struct remove_if_mdview : details::mdview_facade<remove_if_mdview<Rng, Fun>>
  {
    using cursor = details::remove_if_mdcursor<::ranges::detail::begin_cursor_t<Rng>, Fun>;

    cursor                       begin_cursor() const { return {{}, ::ranges::range_access::begin_cursor(m_rng), m_fun}; }
    ::ranges::default_sentinel_t end_cursor() const { return {}; }

    remove_if_mdview() = default;
    remove_if_mdview(Rng rng, Fun fun)
      : m_rng{std::move(rng)}
      , m_fun{std::move(fun)}
    {
    }

  private:
    Rng                              m_rng;
    ::ranges::semiregular_box_t<Fun> m_fun;
  };

} // namespace mln::ranges
