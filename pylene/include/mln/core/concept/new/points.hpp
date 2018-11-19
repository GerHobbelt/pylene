#pragma once

#include <mln/core/concept/new/stl/fundamentals.hpp>

namespace mln::core::concepts
{

  // clang-format off

  // SemiregularValue
  template<typename Pnt>
  concept bool Point =
    stl::Regular<Pnt> &&
    stl::StrictTotallyOrdered<Pnt>;


  // clang-format on

} // namespace mln::core::concepts
