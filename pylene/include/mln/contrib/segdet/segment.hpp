#pragma once

#include <utility>
#include <vector>

namespace mln
{
  namespace contrib
  {
    namespace segdet
    {
      struct Parameters
      {
        u_int32_t nb_values_to_keep;
        u_int32_t variance_position;
        u_int32_t variance_thickness;
        u_int32_t variance_luminosity;

        u_int32_t default_sigma_position;
        u_int32_t default_sigma_thickness;
        u_int32_t default_sigma_luminosity;

        u_int32_t min_nb_values_sigma;
        double    sigma_pos_min;
        double    sigma_thickness_min;
        double    sigma_luminosity_min;

        double    slope_max_vertical;
        double    slope_max_horizontal;
        u_int32_t max_llum;
        u_int32_t max_thickness;
        double    ratio_lum;
        double    merge_slope_variation;
        double    merge_distance_max;
        u_int32_t max_slopes_too_large;
        double    threshold_intersection;
        u_int32_t minimum_for_fusion;

        Parameters()
            : nb_values_to_keep(30)
            , variance_position(2)
            , variance_thickness(1)
            , variance_luminosity(12)
            , default_sigma_position(2)
            , default_sigma_thickness(2)
            , default_sigma_luminosity(57)
            , min_nb_values_sigma(10)
            , sigma_pos_min(1)
            , sigma_thickness_min(0.64)
            , sigma_luminosity_min(13)
            , slope_max_vertical(1.05)
            , slope_max_horizontal(1.0)
            , max_llum(225)
            , max_thickness(100)
            , ratio_lum(1)
            , merge_slope_variation(0.4)
            , merge_distance_max(8)
            , max_slopes_too_large(5)
            , threshold_intersection(0.8)
            , minimum_for_fusion(15)
        {
        }
      };

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
          , length( 1 + (isHorizontal ? points[points.size() - 1].x - points[0].x : points[points.size() - 1].y - points[0].y))
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
