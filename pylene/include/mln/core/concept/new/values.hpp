#pragma once

#include <mln/core/concept/new/stl/fundamentals.hpp>

namespace mln::core::concepts
{

  // clang-format off

  // SemiregularValue
  template <typename SemiregVal>
  concept bool SemiregularValue = stl::Semiregular<SemiregVal>;


  // Value
  template <typename Val>
  concept bool Value = SemiregularValue<Val>;


  // RegularValue
  template <typename RegVal>
  concept bool RegularValue =
    SemiregularValue<RegVal> &&
    stl::Regular<RegVal>;


  // StrictTotallyOrderedRegularValue
  template <typename STORegVal>
  concept bool StrictTotallyOrderedRegularValue =
    RegularValue<STORegVal> &&
    stl::StrictTotallyOrdered<STORegVal>;

  // clang-format on

} // namespace mln::core::concepts
