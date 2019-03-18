#pragma once

#include <mln/core/rangev3/range_traits.hpp>

#include <range/v3/view/single.hpp>

namespace mln::ranges
{

  /// \function rows
  /// \brief Provide unified way to iterate row-wise over uni-dimensional and multi-dimensional ranges.
  ///
  /// \p rows(rng) returns \p rng.rows() if \p rng is multi-dimensional, \p {rng} otherwise.

  template <class R, typename = std::enable_if_t<is_segmented_range_v<std::remove_reference_t<R>>>>
  decltype(auto) rows(R&& rng) // decltype for perfect forwarding
  {
    static_assert(std::is_lvalue_reference<R>(), "Cannot apply on a temporary range.");
    return rng.rows();
  }

  template <class R, typename = std::enable_if_t<!is_segmented_range_v<std::remove_reference_t<R>>>>
  auto rows(R&& rng)
  {
    static_assert(::ranges::Range<R>(), "The input is not a range.");
    return ::ranges::single_view(std::forward<R>(rng));
  }
} // namespace mln::ranges