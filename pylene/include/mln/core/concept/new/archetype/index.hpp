#pragma once

#include <mln/core/concept/new/indexes.hpp>


namespace mln::concepts::archetype
{

  using index_archetype = int;

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(Index<index_archetype>, "index_archetype does not model the Index concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED

} // namespace mln::concepts::archetype
