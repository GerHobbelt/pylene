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
          , thickness(size)
        {
        }
        Point(u_int32_t n, u_int32_t t, u_int32_t size, bool is_horizontal)
          : thickness(size)
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
        u_int32_t thickness;
      };
      struct Segment
      {

        Segment(std::vector<Point> points_vector, std::vector<Point> underOther, float firstPartSlope, float lastPartSlope,
                bool isHorizontal)
          : points(std::move(points_vector))
          , under_other(std::move(underOther))
          , first_part_slope(firstPartSlope)
          , last_part_slope(lastPartSlope)
          , length(points.size() + under_other.size())
          , is_horizontal(isHorizontal)
        {
          nb_pixels = 0;
          for(auto machin : points)
            nb_pixels += machin.thickness;
          for (auto machin :under_other)
            nb_pixels += machin.thickness;
        }

        std::vector<Point> points;
        std::vector<Point> under_other;

        float     first_part_slope;
        float     last_part_slope;
        u_int32_t length;
        u_int32_t nb_pixels;
        bool      is_horizontal;
      };
    } // namespace segdet
  }   // namespace contrib
} // namespace mln
