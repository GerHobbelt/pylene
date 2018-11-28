#pragma once

#include <mln/core/concept/new/cmcstl2.hpp>

#include <mln/core/rangev3/private/multidimensional_range.hpp>


namespace mln::concepts
{

  // clang-format off

#ifdef PYLENE_CONCEPT_TS_ENABLED
  // SegmentedRange
  template<typename Rng>
  concept bool SegmentedRange = 
    stl::ForwardRange<Rng> &&
    std::is_base_of_v<mln::ranges::multidimensional_range_base, Rng> &&
    requires(const Rng crng) {
        { crng.rows() } -> stl::ForwardRange&&;
    };


  // ReversibleRange
  template<typename Rng>
  concept bool ReversibleRange = 
    stl::ForwardRange<Rng> &&
    requires(const Rng crng) {
        { crng.reversed() } -> stl::ForwardRange&&;
    };
#endif // PYLENE_CONCEPT_TS_ENABLED

  // clang-format on

} // namespace mln::concepts
