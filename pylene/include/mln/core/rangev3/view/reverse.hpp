#pragma once

#include <mln/core/concept/new/concepts.hpp>
#include <mln/core/rangev3/private/reversible_range.hpp>

#include <range/v3/view/reverse.hpp>


// TODO:
// Add a better dispatch based on concept checking
namespace mln::ranges::view
{
  template <class R, std::enable_if_t<is_reversible_range_v<R>, int> = 0>
#ifdef PYLENE_CONCEPT_TS_ENABLED
  // clang-format off
      requires mln::concepts::ReversibleRange<R>
#endif
  auto reverse(R&& rng)
  // clang-format on
  {
    return rng.reversed();
  }

  template <class R, std::enable_if_t<!is_reversible_range_v<R>, int> = 0>
#ifdef PYLENE_CONCEPT_TS_ENABLED
  // clang-format off
      requires mln::concepts::stl::InputRange<R>
#endif
  auto reverse(R&& rng)
  // clang-format on
  {
    return ::ranges::view::reverse(std::forward<R>(rng));
  }

} // namespace mln::ranges::view
