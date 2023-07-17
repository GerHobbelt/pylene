#pragma once


#include <mln/core/range/private/mdrange_facade.hpp>

#include <range/v3/view/zip.hpp>

#include <functional>
#include <tuple>

namespace mln::ranges
{

  namespace details
  {

    struct make_tuple_functor_t
    {
      // See https://github.com/ericniebler/stl2/issues/381#issuecomment-285908567
      // why we can't use std::pair/tuple (the const-ness of operator= is in cause)
      template <typename... V>
      ::ranges::common_tuple<V...> operator()(V&&... vals) const
      {
        // Not "std::tie" because may be prvalue
        // Not "std::forward_as_reference" because may hold dangling reference if xvalue
        return ::ranges::common_tuple<V...>{std::forward<V>(vals)...};
      }

      // Do not use common_pair that do not overload std::get stuffs
      // template <typename V1, typename V2>
      // ::ranges::common_pair<V1, V2> operator()(V1&& a, V2&& b) const
      // {
      //   // Not "std::tie" because may be prvalue
      //   // Not "std::forward_as_reference" because may hold dangling reference if xvalue
      //   return {std::forward<V1>(a), std::forward<V2>(b)};
      // }
    };

    constexpr const inline make_tuple_functor_t make_tuple_functor{};

    template <class F, MDCursor... Cur>
    struct zip_with_mdcursor : mdcursor_facade<zip_with_mdcursor<F, Cur...>>
    {
      using cursor_tuple_t = std::tuple<Cur...>;
      using S              = ::ranges::detail::end_cursor_t<std::tuple_element_t<0, cursor_tuple_t>>;

      struct cursor
      {
        auto read() const
        {
          return std::apply([zip_fn = m_zip_fn](auto&&... cur) { return zip_fn(::ranges::range_access::read(cur)...); },
                            m_cursors);
        }

        void next()
        {
          std::apply([](auto&&... cur) { (::ranges::range_access::next(cur), ...); }, m_cursors);
        }

        bool equal(const cursor& other) const
        {
          return ::ranges::range_access::equal(std::get<0>(m_cursors), std::get<0>(other.m_cursors));
        }

        bool equal(const S& other) const
        {
          return ::ranges::range_access::equal(std::get<0>(m_cursors), other); //
        }

        std::tuple<::ranges::detail::begin_cursor_t<Cur>...> m_cursors;
        [[no_unique_address]] F                              m_zip_fn;
      };

      cursor begin_cursor() const
      {
        auto cursors = std::apply(
            [](auto&&... cur) { return std::make_tuple(::ranges::range_access::begin_cursor(cur)...); }, m_cursors);
        return {cursors, m_zip_fn};
      }

      S end_cursor() const { return ::ranges::range_access::end_cursor(std::get<0>(m_cursors)); }

      void next()
      {
        std::apply([](auto&&... cur) { (::ranges::range_access::next(cur), ...); }, m_cursors);
      }


      bool equal(const zip_with_mdcursor& other) const
      {
        return ::ranges::range_access::equal(std::get<0>(m_cursors), std::get<0>(other.m_cursors));
      }

      bool equal(::ranges::default_sentinel_t) const
      {
        return ::ranges::range_access::equal(std::get<0>(m_cursors), ::ranges::default_sentinel);
      }

      cursor_tuple_t          m_cursors;
      [[no_unique_address]] F m_zip_fn;
    };
  } // namespace details


  template <class F, class... Rng>
  struct zip_with_mdview : details::mdview_facade<zip_with_mdview<F, Rng...>>
  {
    static_assert((... && MDRange<Rng>));
    using cursor = details::zip_with_mdcursor<F, ::ranges::detail::begin_cursor_t<Rng>...>;

    cursor begin_cursor() const
    {
      auto cursors = std::apply(
          [](auto&&... cur) { return std::make_tuple(::ranges::range_access::begin_cursor(cur)...); }, m_ranges);
      return {{}, cursors, m_zip_fn};
    }

    ::ranges::default_sentinel_t end_cursor() const { return {}; }

    zip_with_mdview() = default;
    zip_with_mdview(F zip_fn, Rng... ranges)
      : m_ranges{std::move(ranges)...}
      , m_zip_fn{std::move(zip_fn)}
    {
    }

  private:
    std::tuple<Rng...>      m_ranges;
    [[no_unique_address]] F m_zip_fn;
  };

  template <class F, MDBidirectionalRange... Rng>
  struct zip_with_mdview<F, Rng...> : details::mdview_facade<zip_with_mdview<F, Rng...>>
  {
    using cursor = details::zip_with_mdcursor<F, ::ranges::detail::begin_cursor_t<Rng>...>;
    using backward_cursor = details::zip_with_mdcursor<F, typename Rng::backward_cursor...>;

    cursor begin_cursor() const
    {
      auto cursors = std::apply(
          [](auto&&... cur) { return std::make_tuple(::ranges::range_access::begin_cursor(cur)...); }, m_ranges);
      return {{}, cursors, m_zip_fn};
    }

    ::ranges::default_sentinel_t end_cursor() const { return {}; }

    backward_cursor rbegin_cursor() const
    {
      auto cursors = std::apply([](auto&&... cur) { return std::make_tuple(cur.rbegin_cursor()...); }, m_ranges);
      return {{}, cursors, m_zip_fn};
    }

    ::ranges::default_sentinel_t rend_cursor() const { return {}; }


    zip_with_mdview() = default;
    zip_with_mdview(F zip_fn, Rng... ranges)
      : m_ranges{std::move(ranges)...}
      , m_zip_fn{std::move(zip_fn)}
    {
    }

  private:
    std::tuple<Rng...>      m_ranges;
    [[no_unique_address]] F m_zip_fn;
  };

  namespace view
  {
    template <class F, MDRange... Rng>
    [[gnu::always_inline]] inline auto zip_with(F zip_fn, Rng&&... ranges)
    {
      return zip_with_mdview<F, ::concepts::remove_cvref_t<Rng>...>{std::move(zip_fn), std::forward<Rng>(ranges)...};
    }

    template <class F, ::ranges::cpp20::range... Rng>
    [[gnu::always_inline]] inline auto zip_with(F&& zip_fn, Rng&&... ranges)
    {
      return ::ranges::views::zip_with(std::forward<F>(zip_fn), std::forward<Rng>(ranges)...);
    }
  }

  namespace view
  {
    template <MDRange... Rng>
    [[gnu::always_inline]] inline auto zip(Rng&&... ranges)
    {
      return zip_with(mln::ranges::details::make_tuple_functor, std::forward<Rng>(ranges)...);
    }


    template <::ranges::cpp20::range... Rng>
    [[gnu::always_inline]] inline auto zip(Rng&&... ranges)
    {
      return ::ranges::views::zip(std::forward<Rng>(ranges)...);
    }
  }

} // namespace mln::ranges
