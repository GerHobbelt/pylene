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
        {
        }
        Point(u_int32_t n, u_int32_t t, u_int32_t size, bool is_horizontal)
          : size(size)
        {
          if (is_horizontal)
          {
            x = t;
            y = n;
          }
          else
          {
            x = n;
            y = t;
          }
        }

        u_int32_t x;
        u_int32_t y;
        u_int32_t size;
      };
      struct Segment
      {

        Segment(std::vector<Point> points, std::vector<Point> underOther, float firstPartSlope, float lastPartSlope,
                bool isHorizontal)
          : points(std::move(points))
          , under_other(std::move(underOther))
          , first_part_slope(firstPartSlope)
          , last_part_slope(lastPartSlope)
          , length(points.size() + underOther.size())
          , is_horizontal(isHorizontal)
        {
        }

        std::vector<Point> points;
        std::vector<Point> under_other;

        float     first_part_slope;
        float     last_part_slope;
        u_int32_t length;
        bool      is_horizontal;
      };
    } // namespace segdet
  }   // namespace contrib
} // namespace mln
