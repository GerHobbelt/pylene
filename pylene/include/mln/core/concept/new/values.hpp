#pragma once

#include <mln/core/concept/new/cmcstl2.hpp>

namespace mln::concepts
{

  // clang-format off

#ifdef PYLENE_CONCEPT_TS_ENABLED
  // Value
  template <typename Val>
  concept Value = stl::Semiregular<Val>;


  // ComparableValue
  template <typename RegVal>
  concept ComparableValue =
    Value<RegVal> &&
    stl::Regular<RegVal>;


  // OrderedValue
  template <typename STORegVal>
  concept OrderedValue =
    ComparableValue<STORegVal> &&
    stl::StrictTotallyOrdered<STORegVal>;
#endif // PYLENE_CONCEPT_TS_ENABLED

  // clang-format on

} // namespace mln::concepts

// Validate concept
#include <mln/core/concept/new/archetype/value.hpp>
