#pragma once

#include <mln/core/concept/new/points.hpp>


namespace mln::archetypes
{

  struct Point final
  {
  };

  bool operator==(const Point&, const Point&);
  bool operator!=(const Point&, const Point&);
  bool operator<(const Point&, const Point&);
  bool operator>(const Point&, const Point&);
  bool operator<=(const Point&, const Point&);
  bool operator>=(const Point&, const Point&);

#ifdef PYLENE_CONCEPT_TS_ENABLED
  static_assert(mln::concepts::Point<Point>, "Point archetype does not model the Point concept!");
#endif // PYLENE_CONCEPT_TS_ENABLED

} // namespace mln::archetypes
