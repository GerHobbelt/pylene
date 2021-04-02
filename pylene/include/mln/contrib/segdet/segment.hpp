#pragma once

#include <utility>
#include <vector>

namespace mln
{
  namespace contrib
  {
    namespace segdet
    {
      struct Point
      {
        Point(u_int32_t x, u_int32_t y, u_int32_t size)
          : x(x)
          , y(y)
          , size(size)
        {}

        u_int32_t x;
        u_int32_t y;
        u_int32_t size;
      };
      struct Segment
      {

        Segment(std::vector<Point>  points, std::vector<Point>  underOther, float firstPartSlope,
                float lastPartSlope, u_int32_t length, bool isHorizontal)
          : points(std::move(points))
          , under_other(std::move(underOther))
          , first_part_slope(firstPartSlope)
          , last_part_slope(lastPartSlope)
          , length(length)
          , is_horizontal(isHorizontal)
        {}

        std::vector<Point> points;
        std::vector<Point> under_other;

        float first_part_slope;
        float last_part_slope;
        u_int32_t length;
        bool is_horizontal;
      };
    }
  }
}

