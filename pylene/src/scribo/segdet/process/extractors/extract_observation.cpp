#include "extract_observation.hpp"

#include <iostream>
#include <numeric>

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
  Eigen::Matrix<float, 3, 1> determine_observation_binary(const mln::image2d<uint8_t>& image, int& n, int t, int n_max,
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

  Eigen::Matrix<float, 3, 1> determine_observation_gradient(const mln::image2d<uint8_t>& image, int& n, int t,
                                                            int n_max, const Descriptor& descriptor)
  {
    int gradient_sign = static_cast<int>(image({t, n + 1})) - static_cast<int>(image({t, n - 1})) < 0 ? -1 : 1;

    int ithickness = 1;
    while (n + ithickness + 1 < n_max)
    {
      int gradient =
          static_cast<int>(image({t, (n + ithickness) + 1})) - static_cast<int>(image({t, (n + ithickness) - 1}));
      if (gradient * gradient_sign < descriptor.gradient_threshold)
        break;

      ithickness++;
    }

    int grad = static_cast<int>(image({t, n + ithickness})) - static_cast<int>(image({t, n - 1}));

    float position   = (n - 1 + n + ithickness) / 2.0f;
    float thickness  = ithickness;
    float luminosity = grad;

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
          observations.push_back(determine_observation_binary(image, n, t, n_max, descriptor));
        }
      }
      break;
    case SEGDET_PROCESS_EXTRACTION_ENUM::GRADIENT:
      for (int n = 1; n < (n_max - 1); n++)
      {
        if (std::abs(static_cast<int>(image({t, n - 1})) - static_cast<int>(image({t, n + 1}))) >
            descriptor.gradient_threshold)
        {
          observations.push_back(determine_observation_gradient(image, n, t, n_max, descriptor));
        }
      }
      break;
    }

    return observations;
  }
} // namespace scribo::internal