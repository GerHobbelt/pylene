#pragma once

#include <mln/core/rangev3/range_traits.hpp>

#include <range/v3/view/single.hpp>

namespace mln::ranges
{

  /// \function rows
  /// \brief Provide unified way to iterate row-wise over uni-dimensional and multi-dimensional ranges.
  ///
  /// \p rows(rng) returns \p rng.rows() if \p rng is multi-dimensional, \p {rng} otherwise.

  template <class R, std::enable_if_t<is_segmented_range_v<std::remove_reference_t<R>>, void*> = nullptr>
  decltype(auto) rows(R&& rng)
  {
    static_assert(std::is_lvalue_reference<R>(), "Cannot apply on a temporary range.");
    return std::forward<R>(rng).rows();
  }

  template <class R, std::enable_if_t<!is_segmented_range_v<std::remove_reference_t<R>>, void*> = nullptr>
  auto rows(R&& rng)
  {
    static_assert(::ranges::Range<R>(), "The input is not a range.");
    return ::ranges::single_view(std::forward<R>(rng));
  }
} // namespace mln::ranges
