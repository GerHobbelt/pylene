#pragma once

#include <mln/core/concept/new/stl/fundamentals.hpp>

namespace mln::core::concepts
{

  // clang-format off

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

  // clang-format on

} // namespace mln::core::concepts
