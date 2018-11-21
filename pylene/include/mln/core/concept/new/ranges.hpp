#pragma once

#include <mln/core/concept/new/stl/fundamentals.hpp>

#include <mln/core/rangev3/private/multidimensional_range.hpp>


namespace mln::concepts
{

  // clang-format off

  // SegmentedRange
  template<typename Rng>
  concept bool SegmentedRange = 
    stl::Range<Rng> &&
    std::is_base_of_v<mln::ranges::multidimensional_range_base, Rng> &&
    requires(const Rng crng) {
        { crng.rows() } -> stl::Range&&;
    };


  // ReversibleRange
  template<typename Rng>
  concept bool ReversibleRange = 
    stl::Range<Rng> &&
    requires(const Rng crng) {
        { crng.reversed() } -> stl::Range&&;
    };

  // clang-format on

} // namespace mln::concepts
