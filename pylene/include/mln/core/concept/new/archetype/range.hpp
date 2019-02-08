#pragma once

#include <mln/core/concept/new/archetype/value.hpp>
#include <mln/core/concept/new/check.hpp>
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

    PYLENE_CONCEPT_TS_ASSERT(mln::concepts::stl::RandomAccessRange<RandomAccessRange>,
                             "RandomAccessRange archetype does not model the stl::RandomAccessRange concept!");

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

  PYLENE_CONCEPT_TS_ASSERT(mln::concepts::SegmentedRange<SegmentedRange>,
                           "SegmentedRange archetype does not model the SegmentedRange concept!");


  struct ReversibleRange final : detail::RandomAccessRange
  {
    detail::RandomAccessRange reversed();
  };

  PYLENE_CONCEPT_TS_ASSERT(mln::concepts::ReversibleRange<ReversibleRange>,
                           "reversible_range_archetype does not model the ReversibleRange concept!");


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

  PYLENE_CONCEPT_TS_ASSERT(mln::concepts::SegmentedRange<SegmentedAndReversibleRange>,
                           "SegmentedAndReversibleRange archetype does not model the SegmentedRange concept!");
  PYLENE_CONCEPT_TS_ASSERT(mln::concepts::ReversibleRange<SegmentedAndReversibleRange>,
                           "SegmentedAndReversibleRange archetype does not model the ReversibleRange concept!");

} // namespace mln::archetypes
