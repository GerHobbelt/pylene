#pragma once

#include <mln/core/concept/new/check.hpp>
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

  PYLENE_CONCEPT_TS_ASSERT(mln::concepts::Point<Point>, "Point archetype does not model the Point concept!");

} // namespace mln::archetypes
