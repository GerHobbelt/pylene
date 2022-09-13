#include "extract_observation.hpp"

#include <numeric>

namespace scribo::internal
{

  template <typename T>
  float mean(std::vector<T> array, int start, int end)
  {
    auto it_end = end == 0 ? array.end() : array.begin() + end;
    return std::accumulate(array.begin() + start, it_end, 0.0) / static_cast<double>(array.size());
  }

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
    int thickness = 0;
    int n_max_lum = 0;

    std::vector<std::uint16_t> luminosities_list;
    int                        lumi;

    // n + thickess: current position in the n-th line
    while (n + thickness < n_max && (lumi = (int)image({t, n + thickness})) < descriptor.max_max_llum)
    {
      luminosities_list.push_back(lumi);

      if (lumi < luminosities_list[n_max_lum])
        n_max_lum = thickness;

      thickness += 1;
    }

    int n_to_skip = n + thickness;                // Position of the next n to work on
    int max_lum   = luminosities_list[n_max_lum]; // Max luminosity of the current span

    // m_lum : max_luminosity of what is accepted in the span
    int  m_lum   = max_lum + (descriptor.max_max_llum - max_lum) * descriptor.ratio_lum;
    auto n_start = n;             // n_start is AT LEAST n
    int  n_end   = n + thickness; // n_end is AT MOST n + thickness

    if (n_end == n_max) // In case we stopped because of outOfBound value
      n_end--;

    while (luminosities_list[n - n_start] > m_lum)
      n += 1;

    while (image({t, n_end}) > m_lum)
      n_end--;

    n_end++;

    thickness    = n_end - n;
    int position = n + thickness / 2;

    if (n_end - n > (int)luminosities_list.size())
    {
      thickness--;
      n_end--;
      position = n + thickness / 2;
    }
    const float mean_value = mean(luminosities_list, n - n_start, n_end - n_start);

    n = n_to_skip; // Setting reference value of n

    return Eigen::Matrix<float, 3, 1>(position, thickness, mean_value);
  }

  std::vector<Eigen::Matrix<float, 3, 1>> extract_observations(const mln::image2d<uint8_t>& image, int t, int n_max,
                                                               const Descriptor& descriptor)
  {
    std::vector<Eigen::Matrix<float, 3, 1>> observations;

    switch (descriptor.extraction_type)
    {
    case SEGDET_PROCESS_EXTRACTION_ENUM::BINARY:
      for (int n = 0; n < n_max; n++)
        if (image({t, n}) < descriptor.max_llum)
          observations.push_back(determine_observation(image, n, t, n_max, descriptor));
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
          observations.push_back(Eigen::Matrix<float, 3, 1>(n + size / 2, 1, 1));
        }
      }
    }

    return observations;
  }
} // namespace scribo::internal