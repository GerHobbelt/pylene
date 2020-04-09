#pragma once

#include <mln/core/concepts/range.hpp>
#include <range/v3/view/single.hpp>

namespace mln::ranges
{

  /// \function rows
  /// \brief Provide unified way to iterate row-wise over uni-dimensional and multi-dimensional ranges.
  ///
  /// \p rows(rng) returns \p rng.rows() if \p rng is multi-dimensional, \p {rng} otherwise.

  template <MDRange R>
  decltype(auto) rows(R&& rng)
  {
    static_assert(std::is_lvalue_reference<R>(), "Cannot apply on a temporary range.");
    return rng.rows();
  }

  template <::ranges::cpp20::range R>
  auto rows(R&& rng)
  {
    return ::ranges::single_view(std::forward<R>(rng));
  }
} // namespace mln::ranges
