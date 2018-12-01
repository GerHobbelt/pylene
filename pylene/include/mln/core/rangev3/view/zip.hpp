#pragma once

#include <mln/core/rangev3/view/zip_with.hpp>

#include <mln/core/concept/new/concepts.hpp>

#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/common_tuple.hpp>
#include <range/v3/view/all.hpp>

#include <tuple>


namespace mln::ranges
{

  namespace detail
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
  } // namespace detail

  template <typename... Rngs>
  struct zip_view : zip_with_view<detail::make_tuple_functor_t, Rngs...>
  {
    zip_view()
      : zip_with_view<detail::make_tuple_functor_t, Rngs...>()
    {
    }
    zip_view(Rngs... rngs)
      : zip_with_view<detail::make_tuple_functor_t, Rngs...>(detail::make_tuple_functor, std::move(rngs)...)
    {
    }
  };

  namespace view
  {
    struct zip_fn
    {
      template <typename... Rngs>
      using Concept = ::meta::and_<::ranges::InputRange<Rngs>...>;

      template <typename... Rngs, CONCEPT_REQUIRES_(Concept<Rngs...>())>
#ifdef PYLENE_CONCEPT_TS_ENABLED
      // clang-format off
      requires detail::InputRanges<Rngs...>
#endif
      zip_view<::ranges::view::all_t<Rngs>...> operator()(Rngs&&... rngs) const
      // clang-format on
      {
        return zip_view<::ranges::view::all_t<Rngs>...>{::ranges::view::all(static_cast<Rngs&&>(rngs))...};
      }

#ifndef RANGES_DOXYGEN_INVOKED
      template <typename... Rngs, CONCEPT_REQUIRES_(!Concept<Rngs...>())>
      void operator()(Rngs&&...) const
      {
        CONCEPT_ASSERT_MSG(::meta::and_<::ranges::InputRange<Rngs>...>(),
                           "All of the objects passed to view::zip must model the InputRange "
                           "concept");
      }
#endif
    };

    RANGES_INLINE_VARIABLE(zip_fn, zip)
  } // namespace view

} // namespace mln::ranges

RANGES_SATISFY_BOOST_RANGE(mln::ranges::zip_view)
