#pragma once

#include <mln/core/concept/new/cmcstl2.hpp>

namespace mln::concepts
{

  // clang-format off

#ifdef CONCEPT_TS_ENABLED
  // Value
  template <typename Val>
  concept bool Value = stl::Semiregular<Val>;


  // ComparableValue
  template <typename RegVal>
  concept bool ComparableValue =
    Value<RegVal> &&
    stl::Regular<RegVal>;


  // OrderedValue
  template <typename STORegVal>
  concept bool OrderedValue =
    ComparableValue<STORegVal> &&
    stl::StrictTotallyOrdered<STORegVal>;
#endif

  // clang-format on

} // namespace mln::concepts
