#pragma once

#include <mln/core/rangev3/private/multi_view_facade.hpp>
#include <mln/core/rangev3/private/multidimensional_range.hpp>
#include <range/v3/view/zip.hpp>
#include <range/v3/view_facade.hpp>

#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>

namespace mln::ranges::view
{

  namespace details
  {

    template <typename... Rngs>
    inline constexpr bool is_multidimensional = std::conjunction_v<mln::ranges::is_multidimensional_range<Rngs>...>;

    struct __nothing
    {
    };

    // tuple transform
    template <typename Func, typename... Ts, size_t... I>
    constexpr auto tuple_transform_impl(const std::tuple<Ts...>& tpl, std::index_sequence<I...>, Func&& f)
    {
      return std::make_tuple(std::forward<Func>(f)(std::get<I>(tpl))...);
    }
    struct tuple_transform_t
    {
      template <typename Func, typename... Ts>
      constexpr auto operator()(const std::tuple<Ts...>& tpl, Func&& f) const
      {
        return tuple_transform_impl(tpl, std::index_sequence_for<Ts...>{}, std::forward<Func>(f));
      }
    };
    inline constexpr tuple_transform_t tuple_transform{};

    // any_of_v
    struct any_of_v_t
    {
      template <typename... Ts>
      constexpr bool operator()(Ts... bools) const
      {
        static_assert((std::is_convertible_v<Ts, bool> && ...), "Arguments not convertible to bool.");
        return (bools || ...);
      }
    };
    inline constexpr any_of_v_t any_of_v{};

    // tuple_any_of
    struct tuple_any_of_t
    {
      template <typename Func, typename... Ts>
      constexpr bool operator()(std::tuple<Ts...> tpl, Func&& f) const
      {
        return std::apply(any_of_v, tuple_transform(tpl, std::forward<Func>(f)));
      }
    };
    inline constexpr tuple_any_of_t tuple_any_of{};

    // identitu_func
    struct identity_func_t
    {
      template <typename T>
      constexpr decltype(auto) operator()(T&& t) const
      {
        return std::forward<T>(t);
      }
    };
    inline constexpr identity_func_t identity_func{};

    // tuple_any
    struct tuple_any_t
    {
      template <typename... Ts>
      constexpr bool operator()(const std::tuple<Ts...>& tpl) const
      {
        return tuple_any_of(tpl, identity_func);
      }
    };
    inline constexpr tuple_any_t tuple_any{};

    // tuple_compare
    template <typename Func, typename... Ts, typename... Us, std::size_t... I>
    constexpr auto tuple_zip_two_with_impl(const std::tuple<Ts...>& lhs, const std::tuple<Us...>& rhs, Func&& f,
                                           std::index_sequence<I...>)
    {
      return std::make_tuple(std::forward<Func>(f)(std::get<I>(lhs), std::get<I>(rhs))...);
    };

    struct tuple_zip_two_with_t
    {
      template <typename Func, typename... Ts, typename... Us>
      constexpr auto operator()(const std::tuple<Ts...>& lhs, const std::tuple<Us...>& rhs, Func&& f) const
      {
        static_assert(sizeof...(Ts) == sizeof...(Us), "Tuples don't have the same size!");
        return tuple_zip_two_with_impl(lhs, rhs, std::forward<Func>(f), std::index_sequence_for<Ts...>{});
      };
    };
    inline constexpr tuple_zip_two_with_t tuple_zip_two_with{};

  } // namespace details

  template <typename Fun, typename... Rngs>
  struct zip_with_view
      : ::ranges::view_facade<zip_with_view<Fun, Rngs...>, ::ranges::finite>,
        std::conditional_t<details::is_multidimensional<Rngs...>, multidimensional_range_base, details::__nothing>
  {
  private:
    friend ::ranges::range_access;

    ::ranges::semiregular_t<Fun> f_;
    std::tuple<Rngs...> rngs_;

    struct cursor;

    struct sentinel
    {
    private:
      friend struct cursor;
      std::tuple<::ranges::sentinel_t<Rngs>...> ends_;

    public:
      constexpr sentinel() = default;
      constexpr sentinel(::ranges::sentinel_t<Rngs>... rngs) : ends_(rngs...) {}
    };

    struct cursor
    {
    private:
      using fun_ref_ = ::ranges::semiregular_ref_or_val_t<Fun, true>;
      fun_ref_ fun_;
      std::tuple<::ranges::iterator_t<Rngs>...> begins_;

    public:
      constexpr cursor() = default;
      constexpr explicit cursor(fun_ref_ f, ::ranges::iterator_t<Rngs>... its) : fun_(f), begins_(its...) {}

      constexpr auto read() const
      {
        return std::apply(fun_, details::tuple_transform(begins_, [](const auto& rng_it) { return *rng_it; }));
      }

      constexpr bool equal(const sentinel& s) const
      {
        return details::tuple_any(details::tuple_zip_two_with(begins_, s.ends_, [](auto&& lhs, auto&& rhs) {
          return std::forward<decltype(lhs)>(lhs) == std::forward<decltype(rhs)>(rhs);
        }));
      }

      constexpr void next()
      {
        std::apply([](auto&... rng_it) { (++rng_it, ...); }, begins_);
      }
    };

    constexpr cursor begin_cursor()
    {
      return std::make_from_tuple<cursor>(std::tuple_cat(
          std::make_tuple(f_), details::tuple_transform(rngs_, [](const auto& rng) { return std::begin(rng); })));
    }
    constexpr sentinel end_cursor()
    {
      return std::make_from_tuple<sentinel>(
          details::tuple_transform(rngs_, [](const auto& rng) { return std::end(rng); }));
    }
    constexpr cursor begin_cursor() const
    {
      return std::make_from_tuple<cursor>(std::tuple_cat(
          std::make_tuple(f_), details::tuple_transform(rngs_, [](const auto& rng) { return std::begin(rng); })));
    }
    constexpr sentinel end_cursor() const
    {
      return std::make_from_tuple<sentinel>(
          details::tuple_transform(rngs_, [](const auto& rng) { return std::end(rng); }));
    }

  public:
    constexpr zip_with_view() = default;
    constexpr explicit zip_with_view(Fun f, Rngs... rngs) : f_(std::move(f)), rngs_(std::move(rngs)...) {}

    constexpr auto rows() const
    {
      // return a range of range of tuple of row
      return std::apply(
          [](auto&... rng) {
            // rng is a range of tuple of line
            return ::ranges::view::zip_with(
                [](auto&&... rows) {
                  // transform a tuple of line into a line of tuple
                  return ::ranges::view::zip(rows...);
                },
                rng.rows()...); // rng.rows() return a tuple of line
          },
          rngs_); // for each ranges of range of tuple of line
    }
  };

  template <typename Fun, typename... Rngs>
  constexpr auto zip_with(Fun f, Rngs... rngs)
  {
    return zip_with_view<Fun, Rngs...>(std::move(f), std::move(rngs)...);
  }

} // namespace mln::ranges::view
