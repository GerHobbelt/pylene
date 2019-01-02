#pragma once

#include <mln/core/concept/new/cmcstl2.hpp>

#include <range/v3/utility/iterator_traits.hpp>


namespace mln::concepts
{

  // clang-format off

#ifdef PYLENE_CONCEPT_TS_ENABLED

  namespace detail
  {

    // RangeValueSame
    template <typename Rng, typename Val>
    concept RangeValueTypeSameAs =
      stl::Same< stl::iter_value_t<stl::iterator_t<Rng>>, Val>;
    

    // RangeValueConvertibleTo
    template <typename Rng, typename Val>
    concept RangeValueTypeConvertibleTo =
      stl::ConvertibleTo<stl::iter_value_t<stl::iterator_t<Rng>>, Val>;

  }


  // SegmentedRange
  template<typename Rng>
  concept SegmentedRange = 
    stl::ForwardRange<Rng> &&
    requires(Rng rng) {
      { rng.rows() } -> stl::ForwardRange&&;
      requires detail::RangeValueTypeSameAs<
        stl::iter_value_t<stl::iterator_t<decltype(rng.rows())>>,
        stl::iter_value_t<stl::iterator_t<Rng>>>;
    };


  // ReversibleRange
  template<typename Rng>
  concept ReversibleRange = 
    stl::ForwardRange<Rng> &&
    requires(Rng rng) {
      { rng.reversed() } -> stl::ForwardRange&&;
      requires detail::RangeValueTypeSameAs<
        decltype(rng.reversed()),
        stl::iter_value_t<stl::iterator_t<Rng>>>;
    };

#endif // PYLENE_CONCEPT_TS_ENABLED

  // clang-format on

} // namespace mln::concepts

// Validate concept
#include <mln/core/concept/new/archetype/range.hpp>
