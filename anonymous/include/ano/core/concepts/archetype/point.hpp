#pragma once

// #include <ano/core/concepts/point.hpp>


namespace ano::archetypes
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

} // namespace ano::archetypes
