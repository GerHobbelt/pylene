#pragma once

#include <mln/core/concept/new/cmcstl2.hpp>

#include <range/v3/utility/iterator_traits.hpp>


namespace mln::concepts
{

  // clang-format off

#ifdef PYLENE_CONCEPT_TS_ENABLED
  // SegmentedRange
  template<typename Rng>
  concept bool SegmentedRange = 
    stl::ForwardRange<Rng> &&
    requires(Rng rng) {
        { rng.rows() } -> stl::ForwardRange&&;
        // FIXME
        /*
        requires stl::Same<
        ::ranges::value_type_t<Rng>,
        ::ranges::value_type_t<::ranges::value_type_t<decltype(rng.rows())>>>;
        */
    };


  // ReversibleRange
  template<typename Rng>
  concept bool ReversibleRange = 
    stl::ForwardRange<Rng> &&
    requires(Rng rng) {
        { rng.reversed() } -> stl::ForwardRange&&;
    };
#endif // PYLENE_CONCEPT_TS_ENABLED

  // clang-format on

} // namespace mln::concepts
