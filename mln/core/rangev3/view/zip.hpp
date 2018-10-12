#pragma once

#include <mln/core/rangev3/view/zip_with.hpp>

#include <tuple>

namespace mln::ranges::view
{

  namespace details
  {
    struct make_tuple_functor_t
    {
      template <typename... Rngs>
      std::tuple<Rngs...> operator()(Rngs&&... rngs) const
      {
        // Not "std::tie" because may be prvalue
        // Not "std::forward_as_reference" because may hold dangling reference if xvalue
        return {std::forward<Rngs>(rngs)...};
      }
    };
    constexpr inline make_tuple_functor_t make_tuple_functor{};
  } // namespace details

  template <typename... Rngs>
  struct zip_view : zip_with_view<details::make_tuple_functor_t, Rngs...>
  {
    constexpr zip_view() : zip_with_view<details::make_tuple_functor_t, Rngs...>() {}
    constexpr zip_view(Rngs... rngs)
        : zip_with_view<details::make_tuple_functor_t, Rngs...>(details::make_tuple_functor, std::move(rngs)...)
    {
    }
  };

  template <typename... Rngs>
  constexpr auto zip(Rngs... rngs)
  {
    return zip_view<Rngs...>(std::move(rngs)...);
  }

} // namespace mln::ranges::view
