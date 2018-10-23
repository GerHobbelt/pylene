#pragma once

#include <mln/core/rangev3/private/multidimensional_range.hpp>
#include <range/v3/view/single.hpp>

namespace mln::ranges
{

  /// \function rows
  /// \brief Provide unified way to iterate row-wise over uni-dimensional and multi-dimensional ranges.
  ///
  /// \p rows(rng) returns \p rng.rows() if \p rng is multi-dimensional, \p {rng} otherwise.

  template <class R, typename = std::enable_if_t<is_multidimensional_range_v<std::remove_reference_t<R>>>>
  decltype(auto) rows(R&& rng)  // decltype for perfect forwarding
  {
    return rng.rows();
  }

  template <class R, typename = std::enable_if_t<!is_multidimensional_range_v<std::remove_reference_t<R>>>>
  auto rows(R&& rng)
  {
    return ::ranges::single_view(std::forward<R>(rng));
  }
}
