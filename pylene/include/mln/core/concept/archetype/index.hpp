#pragma once

#include <mln/core/concept/indexes.hpp>


namespace mln::archetypes
{

  using Index = int;

  static_assert(mln::concepts::Index<Index>, "Index archetype does not model the Index concept!");

} // namespace mln::archetypes
