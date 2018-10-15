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
  } // namespace details

  template <typename... Rngs>
  struct zip_view : zip_with_view<details::make_tuple_functor_t, Rngs...>
  {
    zip_view() : zip_with_view<details::make_tuple_functor_t, Rngs...>() {}
    zip_view(Rngs... rngs)
      : zip_with_view<details::make_tuple_functor_t, Rngs...>(details::make_tuple_functor_t(), std::move(rngs)...)
    {
    }
  };

  template <typename... Rngs>
  auto zip(Rngs&&... rngs)
  {
    return zip_view<::ranges::view::all_t<Rngs>...>(::ranges::view::all(std::forward<Rngs>(rngs))...);
  }

} // namespace mln::ranges::view
