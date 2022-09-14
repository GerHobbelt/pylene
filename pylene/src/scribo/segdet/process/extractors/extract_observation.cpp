#include "extract_observation.hpp"

#include <numeric>
#include <iostream>

namespace scribo::internal
{
  /**
   * Determine the observation Matrix
   * @param image
   * @param n
   * @param t
   * @param n_max
   * @param is_horizontal
   * @return Observation Eigen matrix
   */
  Eigen::Matrix<float, 3, 1> determine_observation(const mln::image2d<uint8_t>& image, int& n, int t, int n_max,
                                                   const Descriptor& descriptor)
  {
    int ithickness = 0;

    std::vector<float> luminosities_list;
    int                min_iluminosity = 255;
    int                sum_iluminosity = 0;
    while (n + ithickness < n_max)
    {
      int iluminosity = static_cast<int>(image({t, n + ithickness}));
      if (iluminosity > descriptor.max_max_llum)
        break;

      if (iluminosity < min_iluminosity)
        min_iluminosity = iluminosity;

      luminosities_list.push_back(iluminosity);
      sum_iluminosity += iluminosity;
      ithickness++;
    }

    float medium_luminosity = min_iluminosity + (descriptor.max_max_llum - min_iluminosity) * descriptor.ratio_lum;
    int   n_min_observation = n;
    while (luminosities_list[n_min_observation - n] > medium_luminosity)
      sum_iluminosity -= luminosities_list[n_min_observation++ - n];

    int n_max_observation = n + ithickness;
    while (luminosities_list[--n_max_observation - n] > medium_luminosity)
      sum_iluminosity -= luminosities_list[n_max_observation - n];

    float position   = (n_max_observation + n_min_observation) / 2.0f;
    float thickness  = n_max_observation - n_min_observation + 1.0f;
    float luminosity = sum_iluminosity / thickness;

    auto ret = Eigen::Matrix<float, 3, 1>(position, thickness, luminosity);

    n += ithickness;

    return ret;
  }

  std::vector<Eigen::Matrix<float, 3, 1>> extract_observations(const mln::image2d<uint8_t>& image, int t, int n_max,
                                                               const Descriptor& descriptor)
  {
    std::vector<Eigen::Matrix<float, 3, 1>> observations;

    switch (descriptor.extraction_type)
    {
    case SEGDET_PROCESS_EXTRACTION_ENUM::BINARY:
      for (int n = 0; n < n_max; n++)
      {
        if (image({t, n}) < descriptor.max_llum)
        {
          observations.push_back(determine_observation(image, n, t, n_max, descriptor));
        }
      }
      break;
    case SEGDET_PROCESS_EXTRACTION_ENUM::GRADIENT:
      for (int n = 1; n < n_max - 1; n++)
      {
        if (std::abs(image({t, n - 1}) - image({t, n + 1})) > descriptor.gradient_threshold)
        {
          int size = 1;
          while (n + size + 1 < n_max &&
                 std::abs(image({t, n + size - 1}) - image({t, n + size + 1})) > descriptor.gradient_threshold)
            size++;
          observations.push_back(
              Eigen::Matrix<float, 3, 1>(n + size / 2, size, image({t, n + size - 1}) - image({t, n + size + 1})));
        }
      }
    }

    return observations;
  }
} // namespace scribo::internal