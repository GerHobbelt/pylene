#pragma once

#include <mln/core/concept/new/cmcstl2.hpp>

namespace mln::concepts
{

  // clang-format off

#ifdef CONCEPT_TS_ENABLED
  // Point
  template<typename Pnt>
  concept bool Point =
    stl::Regular<Pnt> &&
    stl::StrictTotallyOrdered<Pnt>;
#endif

  // clang-format on

} // namespace mln::concepts
