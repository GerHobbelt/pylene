#pragma once

#include <mln/core/rangev3/private/multi_view_facade.hpp>
#include <mln/core/rangev3/range_traits.hpp>
#include <mln/core/utils/blank.hpp>

#include <mln/core/concept/new/concepts.hpp>

#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view_facade.hpp>

#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>


namespace mln::ranges
{
  template <typename Fun, typename... Rngs>
  struct zip_with_view : ::ranges::view_facade<zip_with_view<Fun, Rngs...>, ::ranges::finite>
  {
  private:
    friend ::ranges::range_access;

    ::ranges::semiregular_t<Fun> fun_;
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
      static inline constexpr const bool        are_bidir_rngs_ = (::ranges::BidirectionalRange<Rngs>() && ...);

    public:
      cursor() = default;
      explicit cursor(fun_ref_ f, ::ranges::iterator_t<Rngs>... its)
        : fun_(f)
        , begins_(its...)
      {
      }

      // See https://github.com/ericniebler/stl2/issues/381#issuecomment-285908567
      // why we can't use std::pair/tuple (the const-ness of operator= is in cause)
      auto read() const
      {
        auto fun = [this](const auto&... it) { return this->fun_(*it...); };
        return std::apply(fun, begins_);
      }

      bool equal(const sentinel& s) const
      {
        return std::get<0>(begins_) == std::get<0>(s.ends_);
        // return details::tuple_any(details::tuple_zip_two_with(begins_, s.ends_, [](auto&& lhs, auto&& rhs) {
        //   return std::forward<decltype(lhs)>(lhs) == std::forward<decltype(rhs)>(rhs);
        // }));
      }

      bool equal(const cursor& other) const { return std::get<0>(begins_) == std::get<0>(other.begins_); }

#ifdef PYLENE_CONCEPT_TS_ENABLED
      // clang-format off
      void next() requires (mln::concepts::stl::ForwardRange<Rngs> && ...)
      // clang-format on
#else
      template <typename U = void, typename = std::enable_if_t<(::ranges::ForwardRange<Rngs>() && ...), U>>
      void next()
#endif
      {
        std::apply([](auto&... rng_it) { (++rng_it, ...); }, begins_);
      }

#ifdef PYLENE_CONCEPT_TS_ENABLED
      // clang-format off
      void prev() requires (mln::concepts::stl::BidirectionalRange<Rngs> && ...)
      // clang-format on
#else
      template <class U = void, class = std::enable_if_t<(::ranges::BidirectionalRange<Rngs>() && ...), U>>
      void prev()
#endif
      {
        std::apply([](auto&... rng_it) { (--rng_it, ...); }, begins_);
      }
    };

    cursor begin_cursor()
    {
      auto fun = [this](auto&&... args) {
        return cursor(this->fun_, ::ranges::begin(std::forward<decltype(args)>(args))...);
      };
      return std::apply(fun, rngs_);
    }
    sentinel end_cursor()
    {
      auto f = [](auto&&... args) { return sentinel(::ranges::end(std::forward<decltype(args)>(args))...); };
      return std::apply(f, rngs_);
    }

    // Disable const view (do not make sense)
    /*
    cursor begin_cursor() const
    {
      auto fun = [this](auto&&... args) {
        return cursor(this->fun_, ::ranges::begin(std::forward<decltype(args)>(args))...);
      };
      return std::apply(fun, rngs_);
    }
    sentinel end_cursor() const
    {
      auto fun = [](auto&&... args) { return sentinel(::ranges::end(std::forward<decltype(args)>(args))...); };
      return std::apply(fun, rngs_);
    }
    */

  public:
    zip_with_view() = default;
    explicit zip_with_view(Rngs... rngs)
      : fun_(Fun{})
      , rngs_(std::move(rngs)...)
    {
    }
    explicit zip_with_view(Fun f, Rngs... rngs)
      : fun_(std::move(f))
      , rngs_(std::move(rngs)...)
    {
    }


#ifdef PYLENE_CONCEPT_TS_ENABLED
    // clang-format off
    auto rows() const requires (mln::concepts::SegmentedRange<Rngs> && ...);
    // clang-format on
#else
    template <typename U = void, typename = std::enable_if_t<(is_segmented_range_v<Rngs> && ...), U>>
    auto rows() const;
#endif

#ifdef PYLENE_CONCEPT_TS_ENABLED
    // clang-format off
    auto rows() requires (mln::concepts::SegmentedRange<Rngs> && ...);
    // clang-format on
#else
    template <typename U = void, typename = std::enable_if_t<(is_segmented_range_v<Rngs> && ...), U>>
    auto rows();
#endif

#ifdef PYLENE_CONCEPT_TS_ENABLED
    // clang-format off
    auto reversed() const requires (mln::concepts::ReversibleRange<Rngs> && ...);
    // clang-format on
#else
    template <typename U = void, typename = std::enable_if_t<(is_reversible_range_v<Rngs> && ...), U>>
    auto reversed() const;
#endif
  };

  namespace view
  {

#ifdef PYLENE_CONCEPT_TS_ENABLED
    namespace detail
    {
      template <typename... Rngs>
      // clang-format off
      concept InputRanges = (mln::concepts::stl::InputRange<Rngs> && ...);
      // clang-format on
    } // namespace detail
#endif

    struct zip_with_fn
    {
      template <typename Fun, typename... Rngs>
      using Concept = ::meta::and_<::meta::and_<::ranges::InputRange<Rngs>...>, ::ranges::CopyConstructible<Fun>,
                                   ::ranges::Invocable<Fun&, ::ranges::range_reference_t<Rngs>&&...>>;

      template <typename... Rngs, typename Fun, CONCEPT_REQUIRES_(Concept<Fun, Rngs...>())>
#ifdef PYLENE_CONCEPT_TS_ENABLED
      // clang-format off
      requires detail::InputRanges<Rngs...> &&
               mln::concepts::stl::CopyConstructible<Fun> &&
               mln::concepts::stl::CopyConstructible<Fun&, ::ranges::range_reference_t<Rngs>&&...>
#endif
      auto operator()(Fun fun, Rngs&&... rngs) const
      // clang-format on
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
#ifdef PYLENE_CONCEPT_TS_ENABLED
  // clang-format off
      auto zip_with_view<Fun, Rngs...>::rows() const requires (mln::concepts::SegmentedRange<Rngs> && ...)
  // clang-format on
#else
  template <typename U, typename>
  auto zip_with_view<Fun, Rngs...>::rows() const
#endif
  {
    // Zip function for rows
    auto row_zipper = [fun = this->fun_](auto&&... rows) {
      return view::zip_with(fun, std::forward<decltype(rows)>(rows)...);
    };

    // Apply rows-row-zipper on each range after segmentation
    return std::apply([row_zipper](const auto&... rng) { return view::zip_with(row_zipper, rng.rows()...); }, rngs_);
  }


  template <typename Fun, typename... Rngs>
#ifdef PYLENE_CONCEPT_TS_ENABLED
  // clang-format off
      auto zip_with_view<Fun, Rngs...>::rows() requires (mln::concepts::SegmentedRange<Rngs> && ...)
  // clang-format on
#else
  template <typename U, typename>
  auto zip_with_view<Fun, Rngs...>::rows()
#endif
  {
    // Zip function for rows
    auto row_zipper = [fun = this->fun_](auto&&... rows) {
      return view::zip_with(fun, std::forward<decltype(rows)>(rows)...);
    };

    // Apply rows-row-zipper on each range after segmentation
    return std::apply([row_zipper](auto&&... rng) { return view::zip_with(row_zipper, rng.rows()...); }, rngs_);
  }


  template <typename Fun, typename... Rngs>
#ifdef PYLENE_CONCEPT_TS_ENABLED
  // clang-format off
      auto zip_with_view<Fun, Rngs...>::reversed() const requires (mln::concepts::ReversibleRange<Rngs> && ...)
  // clang-format on
#else
  template <typename U, typename>
  auto zip_with_view<Fun, Rngs...>::reversed() const
#endif
  {
    auto reverse_row_zipper = [fun = this->fun_](const auto&... rng) { return view::zip_with(fun, rng.reversed()...); };

    // Apply reverse-row-zipper on each range
    return std::apply(reverse_row_zipper, rngs_);
  }

} // namespace mln::ranges

RANGES_SATISFY_BOOST_RANGE(mln::ranges::zip_with_view)
