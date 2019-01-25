#pragma once

#include <mln/core/concept/new/check.hpp>
#include <mln/core/concept/new/indexes.hpp>


namespace mln::archetypes
{

  using Index = int;

  PYLENE_CONCEPT_TS_ASSERT(mln::concepts::Index<Index>, "Index archetype does not model the Index concept!");

} // namespace mln::archetypes
