#pragma once

#include <ano/core/concepts/indexe.hpp>


namespace ano::archetypes
{

  using Index = int;

  static_assert(ano::concepts::Index<Index>, "Index archetype does not model the Index concept!");

} // namespace ano::archetypes
