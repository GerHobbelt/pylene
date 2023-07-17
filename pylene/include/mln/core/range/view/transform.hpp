#pragma once

#include <mln/core/range/private/mdrange_facade.hpp>

#include <range/v3/view/transform.hpp>

#include <functional>

namespace mln::ranges
{
  namespace details
  {
    template <class Cur, class Fun>
    requires MDCursor<Cur>
    struct transform_mdcursor : mdcursor_facade<transform_mdcursor<Cur, Fun>>
    {
      struct cursor
      {
        using S = ::ranges::detail::end_cursor_t<Cur>;

        decltype(auto) read() const { return std::invoke(m_fun, m_cur.read()); }
        void           next() { m_cur.next(); }
        bool           equal(const cursor& other) const { return m_cur.equal(other.m_cur); }
        bool           equal(const S& s) const { return m_cur.equal(s); }

        ::ranges::detail::begin_cursor_t<Cur> m_cur;
        ::ranges::semiregular_box_t<Fun>      m_fun;
      };


      cursor begin_cursor() const { return {::ranges::range_access::begin_cursor(m_cur), m_fun}; }
      auto   end_cursor() const { return ::ranges::range_access::end_cursor(m_cur); }

      void next() { ::ranges::range_access::next(m_cur); }
      bool equal(const transform_mdcursor& other) const { return ::ranges::range_access::equal(m_cur, other.m_cur); }
      bool equal(::ranges::default_sentinel_t) const { return ::ranges::range_access::equal(m_cur, ::ranges::default_sentinel); }

      Cur                              m_cur;
      ::ranges::semiregular_box_t<Fun> m_fun;
    };

    template <class Cur1, class Cur2, class Fun>
    requires MDCursor<Cur1> && MDCursor<Cur2>
    struct transform2_mdcursor : mdcursor_facade<transform2_mdcursor<Cur1, Cur2, Fun>>
    {
      struct cursor
      {
        using C = ::ranges::detail::begin_cursor_t<Cur1>;
        using S = ::ranges::detail::end_cursor_t<Cur1>;

        decltype(auto) read() const { return std::invoke(m_fun, m_cur1.read(), m_cur2.read()); }
        void           next() { ::ranges::range_access::next(m_cur1); ::ranges::range_access::next(m_cur2); }
        bool           equal(const cursor& other) const { return m_cur1.equal(other.m_cur1); }
        bool           equal(const S& s) const { return m_cur1.equal(s); }


        ::ranges::detail::begin_cursor_t<Cur1> m_cur1;
        ::ranges::detail::begin_cursor_t<Cur2> m_cur2;
        ::ranges::semiregular_box_t<Fun>      m_fun;
      };


      cursor begin_cursor() const
      {
        return {::ranges::range_access::begin_cursor(m_cur1), ::ranges::range_access::begin_cursor(m_cur2), m_fun};
      }

      auto end_cursor() const { return ::ranges::range_access::end_cursor(m_cur1); }

      void next()
      {
        ::ranges::range_access::next(m_cur1);
        ::ranges::range_access::next(m_cur2);
      }

      bool equal(const transform2_mdcursor& other) const { return ::ranges::range_access::equal(m_cur1, other.m_cur1); }
      bool equal(::ranges::default_sentinel_t) const { return ::ranges::range_access::equal(m_cur1, ::ranges::default_sentinel); }



      Cur1                             m_cur1;
      Cur2                             m_cur2;
      ::ranges::semiregular_box_t<Fun> m_fun;
    };

  } // namespace details


  template <class Rng, class Fun>
  requires MDRange<Rng>
  struct transform_mdview : details::mdview_facade<transform_mdview<Rng, Fun>>
  {
    using cursor = details::transform_mdcursor<::ranges::detail::begin_cursor_t<Rng>, Fun>;

    cursor                       begin_cursor() const { return {{}, ::ranges::range_access::begin_cursor(m_rng), m_fun}; }
    ::ranges::default_sentinel_t end_cursor() const { return {}; }

    transform_mdview() = default;
    transform_mdview(Rng rng, Fun fun)
      : m_rng{std::move(rng)}
      , m_fun{std::move(fun)}
    {
    }

  private:
    Rng                              m_rng;
    ::ranges::semiregular_box_t<Fun> m_fun;
  };

  template <class Rng, class Fun>
  requires MDBidirectionalRange<Rng>
  struct transform_mdview<Rng, Fun> : details::mdview_facade<transform_mdview<Rng, Fun>>
  {
    using cursor          = details::transform_mdcursor<::ranges::detail::begin_cursor_t<Rng>, Fun>;
    using backward_cursor = details::transform_mdcursor<typename Rng::backward_cursor, Fun>;

    cursor                       begin_cursor() const { return {{}, ::ranges::range_access::begin_cursor(m_rng), m_fun}; }
    ::ranges::default_sentinel_t end_cursor() const { return {}; }

    backward_cursor              rbegin_cursor() const { return {{}, m_rng.rbegin_cursor(), m_fun}; }
    ::ranges::default_sentinel_t rend_cursor() const { return {}; }


    transform_mdview() = default;
    transform_mdview(Rng rng, Fun fun)
      : m_rng{std::move(rng)}
      , m_fun{std::move(fun)}
    {
    }

  private:
    Rng                              m_rng;
    ::ranges::semiregular_box_t<Fun> m_fun;
  };

  template <class Rng1, class Rng2, class Fun>
  requires MDRange<Rng1> && MDRange<Rng2>
  struct transform2_mdview : details::mdview_facade<transform2_mdview<Rng1, Rng2, Fun>>
  {
    using cursor = details::transform2_mdcursor<::ranges::detail::begin_cursor_t<Rng1>,
                                                ::ranges::detail::begin_cursor_t<Rng2>, Fun>;

    cursor begin_cursor() const
    {
      return {{}, ::ranges::range_access::begin_cursor(m_rng1), ::ranges::range_access::begin_cursor(m_rng2), m_fun};
    }
    ::ranges::default_sentinel_t end_cursor() const { return {}; }

    transform2_mdview() = default;
    transform2_mdview(Rng1 rng1, Rng2 rng2, Fun fun)
      : m_rng1{std::move(rng1)}
      , m_rng2{std::move(rng2)}
      , m_fun{std::move(fun)}
    {
    }

  private:
    Rng1                              m_rng1;
    Rng2                              m_rng2;
    ::ranges::semiregular_box_t<Fun>  m_fun;
  };

  template <class Rng1, class Rng2, class Fun>
  requires MDBidirectionalRange<Rng1> && MDBidirectionalRange<Rng2>
  struct transform2_mdview<Rng1, Rng2, Fun> : details::mdview_facade<transform2_mdview<Rng1, Rng2, Fun>>
  {
    using cursor = details::transform2_mdcursor<::ranges::detail::begin_cursor_t<Rng1>,
                                                ::ranges::detail::begin_cursor_t<Rng2>, Fun>;

    using backward_cursor = details::transform2_mdcursor<typename Rng1::backward_cursor, typename Rng2::backward_cursor, Fun>;


    cursor begin_cursor() const
    {
      return {{}, ::ranges::range_access::begin_cursor(m_rng1), ::ranges::range_access::begin_cursor(m_rng2), m_fun};
    }

    backward_cursor rbegin_cursor() const
    {
      return {{}, m_rng1.rbegin_cursor(), m_rng2.rbegin_cursor(), m_fun};
    }

    ::ranges::default_sentinel_t end_cursor() const { return {}; }
    ::ranges::default_sentinel_t rend_cursor() const { return {}; }


    transform2_mdview() = default;
    transform2_mdview(Rng1 rng1, Rng2 rng2, Fun fun)
      : m_rng1{std::move(rng1)}
      , m_rng2{std::move(rng2)}
      , m_fun{std::move(fun)}
    {
    }

  private:
    Rng1                              m_rng1;
    Rng2                              m_rng2;
    ::ranges::semiregular_box_t<Fun>  m_fun;
  };

  namespace view
  {

    template <MDRange Rng, class Fun>
    [[gnu::always_inline]] inline auto transform(Rng&& rng, Fun fun)
    {
      using R = std::remove_cvref_t<Rng>;
      return transform_mdview<R, Fun>{std::forward<Rng>(rng), std::move(fun)};
    }

    template <MDRange Rng1, MDRange Rng2, class Fun>
    [[gnu::always_inline]] inline auto transform(Rng1&& rng1, Rng2&& rng2, Fun fun)
    {
      using R1 = std::remove_cvref_t<Rng1>;
      using R2 = std::remove_cvref_t<Rng2>;
      return transform2_mdview<R1, R2, Fun>{std::forward<Rng1>(rng1), std::forward<Rng2>(rng2), std::move(fun)};
    }


    template <::ranges::cpp20::range Rng, class Fun>
    [[gnu::always_inline]] inline auto transform(Rng&& rng, Fun&& fun)
    {
      return ::ranges::views::transform(std::forward<Rng>(rng), std::forward<Fun>(fun));
    }

    template <::ranges::cpp20::range Rng1, ::ranges::cpp20::range Rng2, class Fun>
    [[gnu::always_inline]] inline auto transform(Rng1&& rng1, Rng2&& rng2, Fun&& fun)
    {
      return ::ranges::views::transform(std::forward<Rng1>(rng1), std::forward<Rng2>(rng2), std::forward<Fun>(fun));
    }

  } // namespace view

} // namespace mln::ranges

