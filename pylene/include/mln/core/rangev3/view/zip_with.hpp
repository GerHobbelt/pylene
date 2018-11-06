#pragma once

#include <mln/core/rangev3/private/multi_view_facade.hpp>
#include <mln/core/rangev3/private/multidimensional_range.hpp>
#include <mln/core/rangev3/range_traits.hpp>
#include <mln/core/utils/blank.hpp>

#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/zip.hpp>
#include <range/v3/view_facade.hpp>

#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>


namespace mln::ranges
{

  template <typename Fun, typename... Rngs>
  struct zip_with_view : ::ranges::view_facade<zip_with_view<Fun, Rngs...>, ::ranges::finite>,
                         std::conditional_t<std::conjunction_v<is_multidimensional_range<Rngs>...>,
                                            multidimensional_range_base, mln::details::blank>
  {
  private:
    friend ::ranges::range_access;

    ::ranges::semiregular_t<Fun> f_;
    std::tuple<Rngs...>          rngs_;

    struct cursor;

    struct sentinel
    {
    private:
      friend struct cursor;
      std::tuple<::ranges::sentinel_t<Rngs>...> ends_;

    public:
      sentinel() = default;
      sentinel(::ranges::sentinel_t<Rngs>... rngs)
        : ends_(rngs...)
      {
      }
    };

    struct cursor
    {
    private:
      using fun_ref_ = ::ranges::semiregular_ref_or_val_t<Fun, true>;
      fun_ref_                                  fun_;
      std::tuple<::ranges::iterator_t<Rngs>...> begins_;

    public:
      cursor() = default;
      explicit cursor(fun_ref_ f, ::ranges::iterator_t<Rngs>... its)
        : fun_(f)
        , begins_(its...)
      {
      }

      auto read() const
      {
        auto f = [this](const auto&... it) { return this->fun_(*it...); };
        return std::apply(f, begins_);
      }

      bool equal(const sentinel& s) const
      {
        return std::get<0>(begins_) == std::get<0>(s.ends_);
        // return details::tuple_any(details::tuple_zip_two_with(begins_, s.ends_, [](auto&& lhs, auto&& rhs) {
        //   return std::forward<decltype(lhs)>(lhs) == std::forward<decltype(rhs)>(rhs);
        // }));
      }

      void next()
      {
        std::apply([](auto&... rng_it) { (++rng_it, ...); }, begins_);
      }
    };

    cursor begin_cursor()
    {
      auto f = [this](auto&&... args) {
        return cursor(this->f_, ::ranges::begin(std::forward<decltype(args)>(args))...);
      };
      return std::apply(f, rngs_);
    }
    sentinel end_cursor()
    {
      auto f = [](auto&&... args) { return sentinel(::ranges::end(std::forward<decltype(args)>(args))...); };
      return std::apply(f, rngs_);
    }
    cursor begin_cursor() const
    {
      auto f = [this](auto&&... args) {
        return cursor(this->f_, ::ranges::begin(std::forward<decltype(args)>(args))...);
      };
      return std::apply(f, rngs_);
    }
    sentinel end_cursor() const
    {
      auto f = [](auto&&... args) { return sentinel(::ranges::end(std::forward<decltype(args)>(args))...); };
      return std::apply(f, rngs_);
    }

  public:
    zip_with_view() = default;
    explicit zip_with_view(Fun f, Rngs... rngs)
      : f_(std::move(f))
      , rngs_(std::move(rngs)...)
    {
    }

    template <typename U = void, typename = std::enable_if_t<std::conjunction_v<is_multidimensional_range<Rngs>...>, U>>
    auto rows() const;
  };

  namespace view
  {
    struct zip_with_fn
    {
      template <typename Fun, typename... Rngs>
      using Concept = ::meta::and_<::meta::and_<::ranges::InputRange<Rngs>...>, ::ranges::CopyConstructible<Fun>,
                                   ::ranges::Invocable<Fun&, ::ranges::range_reference_t<Rngs>&&...>>;

      template <typename... Rngs, typename Fun, CONCEPT_REQUIRES_(Concept<Fun, Rngs...>())>
      auto operator()(Fun fun, Rngs&&... rngs) const
      {
        return zip_with_view<Fun, ::ranges::view::all_t<Rngs>...>{std::move(fun),
                                                                  ::ranges::view::all(static_cast<Rngs&&>(rngs))...};
      }

#ifndef RANGES_DOXYGEN_INVOKED
      template <typename Fun, typename... Rngs, CONCEPT_REQUIRES_(!Concept<Fun, Rngs...>())>
      void operator()(Fun, Rngs&&...) const
      {
        CONCEPT_ASSERT_MSG(::meta::and_<::ranges::InputRange<Rngs>...>(),
                           "All of the objects passed to view::zip_with must model the InputRange "
                           "concept");
        CONCEPT_ASSERT_MSG(::ranges::CopyConstructible<Fun>(),
                           "The function object passed to view::zip_with must be CopyConstructible.");
        CONCEPT_ASSERT_MSG(::ranges::Invocable<Fun&, ::ranges::range_reference_t<Rngs>&&...>(),
                           "The function passed to view::zip_with must be callable with arguments "
                           "of the ranges' reference types.");
      }
#endif
    };

    RANGES_INLINE_VARIABLE(zip_with_fn, zip_with)
  } // namespace view


  template <typename Fun, typename... Rngs>
  template <typename U, typename>
  auto zip_with_view<Fun, Rngs...>::rows() const
  {
    // Zip function for rows
    auto row_zipper = [fun = this->f_](auto&&... rows) {
      return view::zip_with(fun, std::forward<decltype(rows)>(rows)...);
    };

    // Apply row-zipper on each range
    return std::apply([row_zipper](const auto&... rng) { return view::zip_with(row_zipper, rng.rows()...); }, rngs_);
  }

} // namespace mln::ranges

RANGES_SATISFY_BOOST_RANGE(mln::ranges::zip_with_view)
