#pragma once

#include <mln/core/concept/new/points.hpp>


namespace mln::concepts::archetype
{

  struct point_archetype final
  {
  };

  bool operator==(const point_archetype&, const point_archetype&);
  bool operator!=(const point_archetype&, const point_archetype&);
  bool operator<(const point_archetype&, const point_archetype&);
  bool operator>(const point_archetype&, const point_archetype&);
  bool operator<=(const point_archetype&, const point_archetype&);
  bool operator>=(const point_archetype&, const point_archetype&);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(Point<point_archetype>, "point_archetype does not model the Point concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED

} // namespace mln::concepts::archetype
