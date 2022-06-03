#pragma once

#include <mln/core/image/ndimage_fwd.hpp>

#include <cstdint>
#include <vector>

namespace scribo
{
  struct SegDetParams
  {
    float     dyn               = 0.6f;
    int nb_values_to_keep = 30;
    int discontinuity     = 0;

    int default_sigma_position   = 2;
    int default_sigma_thickness  = 2;
    int default_sigma_luminosity = 57;

    int min_nb_values_sigma  = 10;
    float     sigma_pos_min        = 1.f;
    float     sigma_thickness_min  = 0.64f;
    float     sigma_luminosity_min = 13.f;

    int max_llum      = 225;
    int max_thickness = 100;

    float ratio_lum = 1.f;

    float merge_slope_variation = 0.4f;
    float merge_distance_max    = 8.f;

    float threshold_intersection = 0.8f;

    int minimum_for_fusion = 15;

    bool is_valid() const;
  };

  struct segdet_output
  {
    std::vector<int>   seg_ids;
    std::vector<float> mid_pos_x;
    std::vector<float> mid_pos_y;
    std::vector<int>   thickness;
    std::vector<float> angle;
  };

  /**
   * Detects lines in the given image trying to find good parameters
   * @param image A grayscale image to process
   * @param min_len The minimum length of segments to detect
   * @return A vector of detected segments
   */
  segdet_output detect_line(const mln::image2d<std::uint8_t>& image, int min_len);

  /**
   * Detects lines in the given image using given parameters
   * @param image A grayscale image to process
   * @param min_len The minimum length of segments to detect
   * @param params A struct containing the parameters of the method
   * @return A vector of detected segments
   */
  segdet_output detect_line(const mln::image2d<std::uint8_t>& image, int min_len, const SegDetParams& params);
} // namespace mln::contrib::segdet