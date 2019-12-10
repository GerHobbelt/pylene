#pragma once

#include <concepts/concepts.hpp>
#include <range/v3/range/concepts.hpp>


namespace mln::concepts
{

  // clang-format off

#ifdef PYLENE_CONCEPT_TS_ENABLED

  namespace detail
  {

    // RangeValueSame
    template <typename Rng, typename Val>
    concept RangeValueTypeSameAs =
      ::concepts::same_as<::ranges::range_value_t<Rng>, Val>;


    // RangeValueConvertibleTo
    template <typename Rng, typename Val>
    concept RangeValueTypeConvertibleTo =
      ::concepts::convertible_to<::ranges::range_value_t<Rng>, Val>;

  }


  // SegmentedRange
  template <typename Rng>
  concept SegmentedRange =
    ::ranges::forward_range<Rng> &&
    requires(Rng rng) {
    { rng.rows() } -> ::ranges::forward_range;
    requires ::concepts::same_as<
      ::ranges::range_value_t<decltype(rng.rows())>,
      ::ranges::range_value_t<Rng>>;
    };


  // ReversibleRange
  template <typename Rng>
  concept ReversibleRange = 
    ::ranges::cpp20::forward_range<Rng> &&
    requires(Rng rng) {
    { rng.reversed() } -> ::ranges::cpp20::forward_range;
      requires ::concepts::same_as<
        ::ranges::range_value_t<decltype(rng.reversed())>,
        ::ranges::range_value_t<Rng>>;
    };

#endif // PYLENE_CONCEPT_TS_ENABLED

  // clang-format on

} // namespace mln::concepts
