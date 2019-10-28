#pragma once

#include <concepts/concepts.hpp>

namespace mln::concepts
{

  // clang-format off

#ifdef PYLENE_CONCEPT_TS_ENABLED
  // Point
  template <typename Pnt>
  concept Point =
    ::concepts::regular<Pnt> &&
    ::concepts::totally_ordered<Pnt>;
#endif // PYLENE_CONCEPT_TS_ENABLED

  // clang-format on

} // namespace mln::concepts

// Validate concept
#include <mln/core/concept/new/archetype/point.hpp>
