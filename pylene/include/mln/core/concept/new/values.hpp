#pragma once

#include <concepts/concepts.hpp>

namespace mln::concepts
{

  // clang-format off

#ifdef PYLENE_CONCEPT_TS_ENABLED
  // Value
  template <typename Val>
  concept Value = ::concepts::semiregular<Val>;


  // ComparableValue
  template <typename RegVal>
  concept ComparableValue =
    ::concepts::regular<RegVal>;


  // OrderedValue
  template <typename STORegVal>
  concept OrderedValue =
    ::concepts::regular<STORegVal> &&
    ::concepts::totally_ordered<STORegVal>;
#endif // PYLENE_CONCEPT_TS_ENABLED

  // clang-format on

} // namespace mln::concepts

// Validate concept
#include <mln/core/concept/new/archetype/value.hpp>
