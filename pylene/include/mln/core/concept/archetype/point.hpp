#pragma once

#include <mln/core/concept/points.hpp>


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

  static_assert(mln::concepts::Point<Point>, "Point archetype does not model the Point concept!");

} // namespace mln::archetypes
