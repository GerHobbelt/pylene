#pragma once

#define GET_VARIABLE_NAME(Variable) (#Variable)
#include "parameters.hpp"
#include <cassert>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace mln::contrib::segdet
{
  struct Point
  {
    Point(u_int32_t t, u_int32_t n, u_int32_t size)
      : thickness(size)
    {
      x = t;
      y = n;
    }

    u_int32_t x;
    u_int32_t y;
    u_int32_t thickness;
  };

  struct Segment
  {
    Segment(std::vector<Point> points_vector, const std::vector<Point>& underOther, float firstPartSlope,
            float lastPartSlope)
      : first_point(points_vector[0])
      , last_point(points_vector[points_vector.size() - 1])
      , points(std::move(points_vector))
      , first_part_slope(firstPartSlope)
      , last_part_slope(lastPartSlope)
      , length(1 + points[points.size() - 1].x - points[0].x)
      , is_horizontal(true)
    {
      nb_pixels = 0;

      for (auto& machin : points)
        nb_pixels += machin.thickness;

      for (auto& machin : underOther)
      {
        nb_pixels += machin.thickness;
        points.push_back(machin);
      }
    }


    Point first_point;
    Point last_point;

    std::vector<Point> points;

    float     first_part_slope;
    float     last_part_slope;
    u_int32_t length;
    u_int32_t nb_pixels;
    bool      is_horizontal;
  };
} // namespace mln::contrib::segdet
