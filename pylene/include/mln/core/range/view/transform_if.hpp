#pragma once

#include <mln/core/range/concepts.hpp>
#include <mln/core/range/private/mdrange_facade.hpp>

#include <concepts/type_traits.hpp>
#include <range/v3/functional/concepts.hpp>

#include <range/v3/view/transform.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/zip.hpp>

#include <functional> // invoke
#include <tuple>


namespace mln::ranges
{

  namespace details
  {
    template <class F>
    struct cursors_read_fun
    {
      template <class... C>
      decltype(auto) operator() (C&&... cursors) const
      {
        return std::invoke(m_f, ::ranges::range_access::read(cursors)...);
      }

      ::ranges::semiregular_box_t<F> m_f;
    };


    struct cursors_next
    {
      template <class... C>
      void operator() (C&&... cursors) const
      {
        (::ranges::range_access::next(cursors), ...);
      }
    };




    template <class F, class Pred, MDCursor... Cur>
    struct transform_if_mdcursor : mdcursor_facade<transform_if_mdcursor<F, Pred, Cur...>>
    {
      using cursor_tuple_t = std::tuple<Cur...>;
      using S              = ::ranges::detail::end_cursor_t<std::tuple_element_t<0, cursor_tuple_t>>;
      using map_fun_t      = cursors_read_fun<F>;
      using pred_fun_t     = cursors_read_fun<Pred>;

      struct cursor
      {
        decltype(auto) read() const
        {
          return std::apply(m_map_fn, m_cursors);
        }

        void next()
        {
          do
          {
            std::apply(details::cursors_next{}, m_cursors);
          } while (!::ranges::range_access::equal(std::get<0>(m_cursors), m_end) && !std::apply(m_pred_fn, m_cursors));
        }


        bool equal(const cursor& other) const
        {
          return ::ranges::range_access::equal(std::get<0>(m_cursors), std::get<0>(other.m_cursors));
        }

        bool equal(::ranges::default_sentinel_t) const
        {
          return ::ranges::range_access::equal(std::get<0>(m_cursors), m_end); //
        }

        std::tuple<::ranges::detail::begin_cursor_t<Cur>...> m_cursors;
        S                                                    m_end;
        [[no_unique_address]] map_fun_t                      m_map_fn;
        [[no_unique_address]] pred_fun_t                     m_pred_fn;
      };

      cursor begin_cursor() const
      {
        auto c = std::apply(
            [](auto&&... cur) { return std::make_tuple(::ranges::range_access::begin_cursor(cur)...); }, m_cursors);
        auto e = ::ranges::range_access::end_cursor(std::get<0>(m_cursors));

        for (; !::ranges::range_access::equal(std::get<0>(c), e); std::apply(details::cursors_next{}, c))
          if (std::apply(m_pred_fn, c))
            break;

        return {c, e, m_map_fn, m_pred_fn};
      }

      ::ranges::default_sentinel_t end_cursor() const { return {}; }

      void next()
      {
        std::apply([](auto&&... cur) { (::ranges::range_access::next(cur), ...); }, m_cursors);
      }

      bool equal(const transform_if_mdcursor& other) const
      {
        return ::ranges::range_access::equal(std::get<0>(m_cursors), std::get<0>(other.m_cursors));
      }

      bool equal(::ranges::default_sentinel_t) const
      {
        return ::ranges::range_access::equal(std::get<0>(m_cursors), ::ranges::default_sentinel);
      }


      cursor_tuple_t                   m_cursors;
      [[no_unique_address]] map_fun_t  m_map_fn;
      [[no_unique_address]] pred_fun_t m_pred_fn;
    };

  } // namespace details

  template <class F, class Pred, MDRange... Rng>
  struct transform_if_mdview : details::mdview_facade<transform_if_mdview<F, Pred, Rng...>>
  {
    using cursor = details::transform_if_mdcursor<F, Pred, ::ranges::detail::begin_cursor_t<Rng>...>;

    cursor begin_cursor() const
    {
      auto cursors = std::apply(
          [](auto&&... cur) { return std::make_tuple(::ranges::range_access::begin_cursor(cur)...); }, m_ranges);
      return {{}, cursors, m_map_fn, m_pred_fn};
    }

    ::ranges::default_sentinel_t end_cursor() const { return {}; }

    transform_if_mdview() = default;
    transform_if_mdview(F map_fn, Pred pred_fn, Rng... ranges)
      : m_ranges{std::move(ranges)...}
      , m_map_fn{std::move(map_fn)}
      , m_pred_fn{std::move(pred_fn)}
    {
    }

  private:
    std::tuple<Rng...>                                      m_ranges;
    [[no_unique_address]] ::ranges::semiregular_box_t<F>    m_map_fn;
    [[no_unique_address]] ::ranges::semiregular_box_t<Pred> m_pred_fn;
  };

  namespace view
  {

    template <class F, class Pred, MDRange... Rng>
    [[gnu::always_inline]] inline auto transform_if(F map_fn, Pred pred_fn, Rng&&... ranges)
    {
      static_assert(::ranges::invocable<F, mln::ranges::mdrange_reference_t<Rng>...>);
      static_assert(::ranges::predicate<Pred, mln::ranges::mdrange_value_t<Rng>...>);

      return transform_if_mdview<F, Pred, ::concepts::remove_cvref_t<Rng>...>{std::move(map_fn), std::move(pred_fn),
                                                                              std::forward<Rng>(ranges)...};
    }


    template <class F, class Pred, ::ranges::cpp20::range... Rng>
    [[gnu::always_inline]] inline auto transform_if(F map_fn, Pred pred_fn, Rng&&... ranges)
    {
      static_assert(::ranges::invocable<F, mln::ranges::mdrange_reference_t<Rng>...>);
      static_assert(::ranges::predicate<Pred, mln::ranges::mdrange_value_t<Rng>...>);

      auto z = ::ranges::view::zip(std::forward<Rng>(ranges)...);
      auto f = ::ranges::view::filter(z, [pred = std::move(pred_fn)](auto&& t) { return std::apply(pred, t); });
      auto m = ::ranges::view::transform(
          z, [f = std::move(map_fn)](auto&& t) -> decltype(auto) { return std::apply(f, t); });

      return m;
    }


  } // namespace view
} // namespace mln::ranges
