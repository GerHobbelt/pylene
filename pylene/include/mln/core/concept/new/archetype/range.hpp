#pragma once

#include <mln/core/concept/new/archetype/value.hpp>
#include <mln/core/concept/new/ranges.hpp>

namespace mln::archetypes
{
  namespace detail
  {
    struct RandomAccessRange
    {
      using value_type = Value;
      using reference  = Value&;

      value_type* begin();
      value_type* end();
    };

#ifdef PYLENE_CONCEPT_TS_ENABLED
    static_assert(mln::concepts::stl::RandomAccessRange<RandomAccessRange>,
                  "RandomAccessRange archetype does not model the stl::RandomAccessRange concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED

  } // namespace detail


  struct SegmentedRange final : detail::RandomAccessRange
  {
    struct NestedSegmentedRange final
    {
      using value_type = detail::RandomAccessRange;
      using reference  = detail::RandomAccessRange&;

      detail::RandomAccessRange* begin();
      detail::RandomAccessRange* end();
    };

    NestedSegmentedRange rows();
  };

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::SegmentedRange<SegmentedRange>,
                "SegmentedRange archetype does not model the SegmentedRange concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED


  struct ReversibleRange final : detail::RandomAccessRange
  {
    detail::RandomAccessRange reversed();
  };

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::ReversibleRange<ReversibleRange>,
                "reversible_range_archetype does not model the ReversibleRange concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED


  struct SegmentedAndReversibleRange final : detail::RandomAccessRange
  {
    struct NestedSegmentedRange final
    {
      using value_type = detail::RandomAccessRange;
      using reference  = detail::RandomAccessRange&;
      detail::RandomAccessRange* begin();
      detail::RandomAccessRange* end();
    };

    NestedSegmentedRange      rows();
    detail::RandomAccessRange reversed();
  };

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::SegmentedRange<SegmentedAndReversibleRange>,
                "SegmentedAndReversibleRange archetype does not model the SegmentedRange concept!");
  static_assert(mln::concepts::ReversibleRange<SegmentedAndReversibleRange>,
                "SegmentedAndReversibleRange archetype does not model the ReversibleRange concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED

} // namespace mln::archetypes
