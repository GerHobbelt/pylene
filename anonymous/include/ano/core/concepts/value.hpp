#pragma once


namespace ano::concepts
{

  // Value
  template <typename Val>
  concept Value = ::concepts::semiregular<Val>;


  // ComparableValue
  template <typename RegVal>
  concept ComparableValue = ::concepts::regular<RegVal>;


  // OrderedValue
  template <typename STORegVal>
  concept OrderedValue = ::concepts::regular<STORegVal> && ::concepts::totally_ordered<STORegVal>;

  // clang-format on

} // namespace ano::concepts
