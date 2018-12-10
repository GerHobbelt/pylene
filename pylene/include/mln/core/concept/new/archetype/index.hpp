#pragma once

#include <mln/core/concept/new/indexes.hpp>


namespace mln::archetypes
{

  using Index = int;

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::Index<Index>, "Index archetype does not model the Index concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED

} // namespace mln::archetypes
