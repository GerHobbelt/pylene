#include <mln/contrib/segdet/detect_line.hpp>


#include <Eigen/Dense>

#include <algorithm>
#include <utility>

#include <mln/contrib/segdet/filter.hpp>
#include <mln/contrib/segdet/linearregression.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/image/view/transform.hpp>


namespace mln::contrib::segdet
{
  /**
   * Give the value of the pixel in (n, t) according to traversal direction
   * @param image Image to take pixel from
   * @param n
   * @param t
   * @param is_horizontal
   * @return the value of the pixel
   */
  uint8_t image_at(image2d<uint8_t> image, int t, int n)
  {
    // TODO remove the check done by image.at(,) using image(,)
    return image.at({t, n});
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
  Eigen::Matrix<double, 3, 1> determine_observation(const image2d<uint8_t>& image, uint32_t& n, uint32_t t,
                                                    uint32_t n_max, Parameters params)
  {
    uint32_t thickness = 0;
    uint32_t n_max_lum = 0;

    std::vector<uint8_t> luminosities_list = std::vector<uint8_t>();
    uint32_t             lumi;

    // n + thickess: current position in the n-th line
    while (n + thickness < n_max && (lumi = image_at(image, t, n + thickness)) < params.max_llum)
    {
      luminosities_list.push_back(lumi);

      if (lumi < luminosities_list[n_max_lum])
        n_max_lum = thickness;

      thickness += 1;
    }

    uint32_t n_to_skip = n + thickness;                // Position of the next n to work on
    uint32_t max_lum   = luminosities_list[n_max_lum]; // Max luminosity of the current span

    // m_lum : max_luminosity of what is accepted in the span
    uint32_t m_lum   = max_lum + (params.max_llum - max_lum) * params.ratio_lum;
    auto     n_start = n;             // n_start is AT LEAST n
    uint32_t n_end   = n + thickness; // n_end is AT MOST n + thickness

    if (n_end == n_max) // In case we stopped because of outOfBound value
      n_end--;

    while (luminosities_list[n - n_start] > m_lum)
      n += 1;

    while (image_at(image, t, n_end) > m_lum)
      n_end--;

    n_end++;

    thickness         = n_end - n;
    uint32_t position = n + thickness / 2;

    if (n_end - n > luminosities_list.size())
    {
      thickness--;
      n_end--;
      position = n + thickness / 2;
    }
    const double mean = mln::contrib::segdet::mean(luminosities_list, n - n_start, n_end - n_start);

    n = n_to_skip; // Setting reference value of n

    return Eigen::Matrix<double, 3, 1>(position, thickness, mean);
  }

  /**
   * Say if a value is between two other
   * @param min
   * @param value
   * @param max
   * @return true if the value is between
   */
  inline bool in_between(double min, double value, double max) { return min <= value && value < max; }


  /**
   * Compute list of filters that accept the current Observation, add the filter inside accepted list
   * @param filters Current
   * @param obs Observation to match
   * @param t Current t
   */
  void find_match(std::vector<Filter>& filters, std::vector<Filter*>& accepted, const Eigen::Matrix<double, 3, 1>& obs,
                  const uint32_t& t, const Parameters& params)
  {
    uint32_t obs_thick    = obs(1, 0);
    uint32_t obs_thick_d2 = obs_thick / 2;

    uint32_t obs_n_min = obs(0, 0) - obs_thick_d2;
    if (obs_n_min != 0)
      obs_n_min--;

    uint32_t obs_n_max = obs(0, 0) + obs_thick_d2 + 1;

    for (auto& f : filters)
    {
      if (accepts(f, obs, obs_n_min, obs_n_max, params))
        accepted.push_back(&f);
      else if (f.observation == std::nullopt && in_between(obs_n_min, f.n_min, obs_n_max) &&
               in_between(obs_n_min, f.n_max, obs_n_max) && f.X_predicted(1, 0) < obs_thick)
      {
        f.currently_under_other.emplace_back(t, round(f.X_predicted(0, 0)), round(f.X_predicted(1, 0)));
      }
    }
  }

  /**
   * Check if the filters are following the same observation
   * @param f first filter
   * @param fj second filter
   * @return true if the filters are following the same observation for SEGDET_MINIMUM_FOR_FUSION length
   */
  bool same_observation(const Filter& f, const Filter& fj, Parameters params)
  {
    if (f.n_values.size() < params.minimum_for_fusion || fj.n_values.size() < params.minimum_for_fusion)
      return false;

    for (uint32_t i = 0; i < params.minimum_for_fusion; i++)
    {
      size_t k  = f.thicknesses.size() - 1 - i;
      size_t kj = fj.thicknesses.size() - 1 - i;

      if (f.thicknesses[k] != fj.thicknesses[kj] || f.t_values[k] != fj.t_values[kj] ||
          f.n_values[k] != fj.n_values[kj])
        return false;
    }

    return true;
  }

  /**
   * Extract the segment from a filter
   * @param filter filter from which we create the segment
   * @param min_len
   * @param nb_to_remove number of last pixels to remove
   * @return the segment created calling the constructor
   */
  Segment make_segment_from_filter(Filter& filter, uint32_t min_len, uint32_t nb_to_remove)
  {
    uint32_t last_index = filter.n_values.size() - nb_to_remove;

    for (uint32_t i = 0; i < last_index; i++)
      filter.segment_points.emplace_back(filter.t_values[i], filter.n_values[i], filter.thicknesses[i]);

    auto& point       = filter.segment_points;
    auto& under_other = filter.under_other;

    for (auto& p : filter.currently_under_other)
      under_other.push_back(p);

    auto first_part_slope =
        std::nullopt == filter.first_slope ? filter.slopes[min_len - 1] : filter.first_slope.value();
    auto last_part_slope = filter.slopes[last_index - 1];

    return Segment(point, under_other, first_part_slope, last_part_slope);
  }

  /**
   * Erase a filter adding if needed the extract segment
   * @param filters Current list of filters
   * @param segments Current list of segments
   * @param min_len
   * @param j Index of the filter to erase
   * @param fj Filter to erase
   */
  void erase_filter(std::vector<Filter>& filters, std::vector<Segment>& segments, uint32_t min_len, uint32_t j,
                    Filter& fj, Parameters params)
  {
    if (fj.last_integration - fj.first - params.minimum_for_fusion > min_len)
      segments.push_back(make_segment_from_filter(fj, min_len, params.minimum_for_fusion));

    filters.erase(filters.begin() + j);
  }

  /**
   * Check if a fusion with the current filter is to be done
   * @param filters Current list of filter
   * @param index Index of the current filter
   * @param segments Current list of segment
   * @param min_len
   * @return true if happened
   */
  bool make_potential_fusion(std::vector<Filter>& filters, uint32_t index, std::vector<Segment>& segments,
                             uint32_t min_len, Parameters params)
  {
    auto f = filters[index];

    uint32_t j                          = index + 1;
    bool     current_filter_was_deleted = false;

    while (j < filters.size())
    {
      auto fj = filters[j];
      if (fj.observation != std::nullopt && same_observation(f, fj, params))
      {
        if (f.first < fj.first)
          erase_filter(filters, segments, min_len, j, fj, params);
        else
        {
          current_filter_was_deleted = true;
          erase_filter(filters, segments, min_len, index, f, params);
          break;
        }
      }

      j++;
    }

    return current_filter_was_deleted;
  }

  /**
   * Say if the filter has to continue
   * @param f filter to check
   * @param t current t
   * @param discontinuity discontinuity allowedt
   * @return true if the filter has to continue
   */
  bool filter_has_to_continue(Filter& f, uint32_t t, uint32_t discontinuity)
  {
    if (t - f.last_integration <= discontinuity)
      return true;

    if (!f.currently_under_other.empty())
    {
      auto last_t = f.currently_under_other[f.currently_under_other.size() - 1].x;
      return t - last_t <= discontinuity;
    }

    return false;
  }

  /**
   * Integrate observation for filters that matched an observation
   * Select filters that have to continue
   * Add segment in segments if needed
   * @param filters Current filters
   * @param segments Current segments
   * @param t Current t
   * @param two_matches Current time of two filters matching
   * @param min_len_embryo
   * @param discontinuity
   * @return List of filters that have to continue
   */
  std::vector<Filter> filter_selection(std::vector<Filter>& filters, std::vector<Segment>& segments, uint32_t t,
                                       uint32_t two_matches, uint32_t min_len_embryo, uint32_t discontinuity,
                                       Parameters params)
  {
    std::vector<Filter> filters_to_keep;

    size_t index = 0;
    while (index < filters.size())
    {
      auto f = filters[index];

      if (f.observation != std::nullopt)
      {
        if (two_matches > params.minimum_for_fusion &&
            make_potential_fusion(filters, index, segments, min_len_embryo, params))
          continue;

        integrate(f, t, params);
        compute_sigmas(f, params);

        if (f.nb_current_slopes_over_slope_max > params.max_slopes_too_large)
        {
          if (f.last_integration - f.first - params.max_slopes_too_large > min_len_embryo)
            segments.push_back(make_segment_from_filter(f, min_len_embryo, 0));
          index++;
          continue;
        }

        filters_to_keep.push_back(f);
      }
      else if (filter_has_to_continue(f, t, discontinuity))
      {
        f.S = f.S_predicted;
        filters_to_keep.push_back(f);
      }
      else if (f.last_integration - f.first > min_len_embryo)
        segments.push_back(make_segment_from_filter(f, min_len_embryo, 0));

      index++;
    }

    return filters_to_keep;
  }

  /**
   * Add last segment from current filters if needed
   * @param filters Current filters
   * @param segments Current segments
   * @param min_len
   */
  void finish_traversal(std::vector<Filter>& filters, std::vector<Segment>& segments, uint32_t min_len,
                        Parameters params)
  {
    uint32_t i = 0;
    while (i < filters.size())
    {
      auto& f = filters[i];
      if (make_potential_fusion(filters, i, segments, min_len, params))
        continue;

      if (f.last_integration - f.first > min_len)
        segments.push_back(make_segment_from_filter(f, min_len, 0));

      i++;
    }
  }

  /**
   * Handle case where one or more filter matched the observation
   * @param new_filters List of new filters
   * @param accepted List of filters that accepts the observation
   * @param obs
   * @param t
   * @param is_horizontal
   * @param slope_max
   * @return
   */
  bool handle_find_filter(std::vector<Filter>& new_filters, std::vector<Filter*>& accepted,
                          const Eigen::Matrix<double, 3, 1>& obs, uint32_t& t, double slope_max)
  {
    auto observation_s = Observation(obs, accepted.size(), t);

    for (auto& f : accepted)
    {
      auto obs_result = choose_nearest(*f, observation_s);

      if (obs_result != std::nullopt)
      {
        auto obs_result_value = obs_result.value();
        obs_result_value.match_count--;

        if (obs_result_value.match_count == 0)
          new_filters.emplace_back(t, slope_max, obs_result_value.obs);
      }
    }

    return observation_s.match_count > 1;
  }

  /**
   * Merge selection and new_filters in filters
   * @param filters
   * @param selection
   * @param new_filters
   */
  void update_current_filters(std::vector<Filter>& filters, std::vector<Filter>& selection,
                              std::vector<Filter>& new_filters)
  {
    filters.clear();

    filters.insert(filters.end(), selection.begin(), selection.end());
    filters.insert(filters.end(), new_filters.begin(), new_filters.end());
  }

  std::vector<Segment> traversal(const image2d<uint8_t>& image, double slope_max, uint min_len_embryo,
                                 uint discontinuity, Parameters params)
  {
    uint32_t n_max = image.size(1), t_max = image.size(0);

    auto                filters  = std::vector<Filter>();  // List of current filters
    auto                segments = std::vector<Segment>(); // List of current segments
    std::vector<Filter> new_filters{};
    std::vector<Filter> selection{};

    uint32_t two_matches = 0; // Number of t where two segments matched the same observation
    // Useful to NOT check if filters has to be merged

    for (uint32_t t = 0; t < t_max; t++)
    {
      for (auto& filter : filters)
        predict(filter);

      new_filters.clear();
      bool two_matches_through_n = false;

      for (uint32_t n = 0; n < n_max; n++)
      {
        if (image_at(image, t, n) < params.max_llum)
        {
          Eigen::Matrix<double, 3, 1> obs = determine_observation(image, n, t, n_max, params);

          std::vector<Filter*> accepted{}; // List of accepted filters by the current observation obs
          find_match(filters, accepted, obs, t, params);
          if (accepted.empty() && obs(1, 0) < params.max_thickness)
            new_filters.emplace_back(t, slope_max, obs);
          else
            two_matches_through_n =
                handle_find_filter(new_filters, accepted, obs, t, slope_max) || two_matches_through_n;
        }
      }

      if (two_matches_through_n)
        two_matches++;
      else
        two_matches = 0;

      // Selection for next turn
      selection = filter_selection(filters, segments, t, two_matches, min_len_embryo, discontinuity, params);
      // Merge selection and new_filters in filters
      update_current_filters(filters, selection, new_filters);
    }

    finish_traversal(filters, segments, min_len_embryo, params);

    return segments;
  }

  image2d<uint8_t> transpose(image2d<uint8_t> image)
  {
    int              width = image.size(0), height = image.size(1);
    image2d<uint8_t> transpose_image = image2d<uint8_t>(height, width);
    mln::fill(transpose_image, 0);
    for (int x = 0; x < width; ++x)
    {
      for (int y = 0; y < height; ++y)
      {
        transpose_image({y, x}) = image.at({x, y});
      }
    }

    return transpose_image;
  }

  void my_swap(uint32_t& a, uint32_t& b)
  {
    auto tmp = a;
    a        = b;
    b        = tmp;
  }

  void transpose_segments(std::vector<Segment>& segments)
  {
    for (auto& segment : segments)
    {
      for (auto& point : segment.points)
      {
        my_swap(point.x, point.y);
      }

      my_swap(segment.last_point.x, segment.last_point.y);
      my_swap(segment.first_point.x, segment.first_point.y);
      segment.is_horizontal = false;
    }
  }

  /**
   * Compute the two traversals to detect horizontal and vertical segments
   * @param image image to extract segment from
   * @param min_len
   * @param discontinuity
   * @return Pair (horizontal segments,vertical segments)
   */
  std::pair<std::vector<Segment>, std::vector<Segment>> process(image2d<uint8_t> image, uint min_len_embryo,
                                                                uint discontinuity, Parameters params)
  {
    std::vector<Segment> horizontal_segments =
        traversal(image, params.slope_max_horizontal, min_len_embryo, discontinuity, params);

    image2d<uint8_t>     transpose_image = transpose(image);
    std::vector<Segment> vertical_segments =
        traversal(transpose_image, params.slope_max_vertical, min_len_embryo, discontinuity, params);
    transpose_segments(vertical_segments);

    return std::make_pair(horizontal_segments, vertical_segments);
  }
} // namespace mln::contrib::segdet