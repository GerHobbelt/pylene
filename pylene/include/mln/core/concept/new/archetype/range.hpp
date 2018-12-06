#pragma once

#include <mln/core/concept/new/archetype/value.hpp>
#include <mln/core/concept/new/ranges.hpp>

namespace mln::concepts::archetype
{
  namespace detail
  {
    struct randomaccess_range_archetype
    {
      using value_type = value_archetype;
      using reference  = value_archetype&;

      value_type* begin();
      value_type* end();
    };

#ifdef PYLENE_CONCEPT_TS_ENABLED
    static_assert(stl::RandomAccessRange<randomaccess_range_archetype>,
                  "randomaccess_range_archetype does not model the stl::RandomAccessRange concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED

  } // namespace detail


  struct segmented_range_archetype final : detail::randomaccess_range_archetype
  {
    struct nested_segmented_range_archetype final
    {
      using value_type = detail::randomaccess_range_archetype;
      using reference  = detail::randomaccess_range_archetype&;

      detail::randomaccess_range_archetype* begin();
      detail::randomaccess_range_archetype* end();
    };

    nested_segmented_range_archetype rows();
  };

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(SegmentedRange<segmented_range_archetype>,
                "segmented_range_archetype does not model the SegmentedRange concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED


  struct reversible_range_archetype final : detail::randomaccess_range_archetype
  {
    detail::randomaccess_range_archetype reversed();
  };

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(ReversibleRange<reversible_range_archetype>,
                "reversible_range_archetype does not model the ReversibleRange concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED


  struct segmented_and_reversible_range_archetype final : detail::randomaccess_range_archetype
  {
    struct nested_segmented_range_archetype final
    {
      using value_type = detail::randomaccess_range_archetype;
      using reference  = detail::randomaccess_range_archetype&;
      detail::randomaccess_range_archetype* begin();
      detail::randomaccess_range_archetype* end();
    };

    nested_segmented_range_archetype     rows();
    detail::randomaccess_range_archetype reversed();
  };

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(SegmentedRange<segmented_and_reversible_range_archetype>,
                "segmented_and_reversible_range_archetype does not model the SegmentedRange concept!");
  static_assert(ReversibleRange<segmented_and_reversible_range_archetype>,
                "segmented_and_reversible_range_archetype does not model the ReversibleRange concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED

} // namespace mln::concepts::archetype
