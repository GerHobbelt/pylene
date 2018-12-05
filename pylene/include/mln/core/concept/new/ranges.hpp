#pragma once

#include <mln/core/concept/new/cmcstl2.hpp>

#include <range/v3/utility/iterator_traits.hpp>


namespace mln::concepts
{

  // clang-format off

#ifdef PYLENE_CONCEPT_TS_ENABLED

  // RangeValueSame
  template <typename Rng, typename V>
  concept bool RangeValueTypeSameAs =
    stl::Same<stl::iter_value_t<stl::iterator_t<Rng>>, V>;
  
  // RangeValueConvertibleTo
  template <typename Rng, typename V>
  concept bool RangeValueTypeConvertibleTo =
    stl::ConvertibleTo<stl::iter_value_t<stl::iterator_t<Rng>>, V>;

  // SegmentedRange
  template<typename Rng>
  concept bool SegmentedRange = 
    stl::ForwardRange<Rng> &&
    requires(Rng rng) {
      { rng.rows() } -> stl::ForwardRange&&;
      requires RangeValueTypeSameAs<
        stl::iter_value_t<stl::iterator_t<decltype(rng.rows())>>,
        stl::iter_value_t<stl::iterator_t<Rng>>>;
    };


  // ReversibleRange
  template<typename Rng>
  concept bool ReversibleRange = 
    stl::ForwardRange<Rng> &&
    requires(Rng rng) {
      { rng.reversed() } -> stl::ForwardRange&&;
      requires RangeValueTypeSameAs<
        decltype(rng.reversed()),
        stl::iter_value_t<stl::iterator_t<Rng>>>;
    };
#endif // PYLENE_CONCEPT_TS_ENABLED

  // clang-format on

} // namespace mln::concepts
