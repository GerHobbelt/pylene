#pragma once

#include <mln/core/rangev3/private/reversible_range.hpp>

#include <range/v3/view/reverse.hpp>


// TODO:
// Add a better dispatch based on concept checking
namespace mln::ranges::view
{
  template <class R, std::enable_if_t<is_reversible_range_v<R>, int> = 0>
  auto reverse(R&& rng)
  {
    return rng.reversed();
  }

  template <class R, std::enable_if_t<!is_reversible_range_v<R>, int> = 0>
  auto reverse(R&& rng)
  {
    return ::ranges::view::reverse(std::forward<R>(rng));
  }

} // namespace mln::ranges::view
