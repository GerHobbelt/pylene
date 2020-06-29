#pragma once

// #include <mln/core/concepts/point.hpp>


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

} // namespace mln::archetypes
