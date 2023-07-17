#pragma once

#include <concepts>

namespace mln::concepts
{

  // Value
  template <typename Val>
  concept Value = std::semiregular<Val>;


  // ComparableValue
  template <typename RegVal>
  concept ComparableValue =
    std::regular<RegVal>;


  // OrderedValue
  template <typename STORegVal>
  concept OrderedValue =
    std::regular<STORegVal> &&
    std::totally_ordered<STORegVal>;

  // clang-format on

} // namespace mln::concepts
