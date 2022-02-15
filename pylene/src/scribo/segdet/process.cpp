#include "detect_line.hpp"
#include "filter.hpp"
#include "label_array.hpp"

#include <mln/core/algorithm/fill.hpp>

#include <Eigen/Dense>
#include <algorithm>

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
  Eigen::Matrix<float, 3, 1> determine_observation(const image2d<uint8_t>& image, int& n, int t, int n_max,
                                                   const SegDetParams& params)
  {
    int thickness = 0;
    int n_max_lum = 0;

    std::vector<std::uint16_t> luminosities_list;
    int                        lumi;

    // n + thickess: current position in the n-th line
    while (n + thickness < n_max && (lumi = (int)image({t, n + thickness})) < params.max_llum)
    {
      luminosities_list.push_back(lumi);

      if (lumi < luminosities_list[n_max_lum])
        n_max_lum = thickness;

      thickness += 1;
    }

    int n_to_skip = n + thickness;                // Position of the next n to work on
    int max_lum   = luminosities_list[n_max_lum]; // Max luminosity of the current span

    // m_lum : max_luminosity of what is accepted in the span
    int  m_lum   = max_lum + (params.max_llum - max_lum) * params.ratio_lum;
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
    const float mean = scribo::internal::mean(luminosities_list, n - n_start, n_end - n_start);

    n = n_to_skip; // Setting reference value of n

    return Eigen::Matrix<float, 3, 1>(position, thickness, mean);
  }

  /**
   * Say if a value is between two other
   * @param min
   * @param value
   * @param max
   * @return true if the value is between
   */
  inline bool in_between(float min, float value, float max) { return min <= value && value < max; }


  /**
   * Compute list of filters that accept the current Observation, add the filter inside accepted list
   * @param filters Current
   * @param obs Observation to match
   * @param t Current t
   */
  void find_match(std::vector<Filter>& filters, std::vector<Filter*>& accepted, const Eigen::Matrix<float, 3, 1>& obs,
                  const int& t, const SegDetParams& params)
  {
    int obs_thick    = obs(1, 0);
    int obs_thick_d2 = obs_thick / 2;

    int obs_n_min = obs(0, 0) - obs_thick_d2;
    if (obs_n_min != 0)
      obs_n_min--;

    int obs_n_max = obs(0, 0) + obs_thick_d2 + 1;

    for (auto& f : filters)
    {
      if (accepts(f, obs, obs_n_min, obs_n_max, params))
        accepted.push_back(&f);
      else if (f.observation == std::nullopt && in_between(obs_n_min, f.n_min, obs_n_max) &&
               in_between(obs_n_min, f.n_max, obs_n_max) && f.X_predicted(1, 0) < obs_thick)
      {
        Span span{};
        span.x         = t;
        span.y         = round(f.X_predicted(0, 0));
        span.thickness = round(f.X_predicted(1, 0));
        span.angle     = std::atan(f.current_slope) * 180 / 3.14;
        f.under_other.push_back(span);
        f.currently_under_other++;
      }
    }
  }

  /**
   * Check if the filters are following the same observation
   * @param f first filter
   * @param fj second filter
   * @return true if the filters are following the same observation for SEGDET_MINIMUM_FOR_FUSION length
   */
  bool same_observation(const Filter& f, const Filter& fj, const SegDetParams& params)
  {
    if (static_cast<int>(f.n_values.size()) < params.minimum_for_fusion ||
        static_cast<int>(fj.n_values.size()) < params.minimum_for_fusion)
      return false;

    for (int i = 0; i < params.minimum_for_fusion; i++)
    {
      int k  = static_cast<int>(f.thicknesses.size()) - 1 - i;
      int kj = static_cast<int>(fj.thicknesses.size()) - 1 - i;

      if (f.thicknesses[k] != fj.thicknesses[kj] || f.t_values[k] != fj.t_values[kj] ||
          f.n_values[k] != fj.n_values[kj])
        return false;
    }

    return true;
  }

  /**
   * Erase a filter adding if needed the extract segment
   * @param filters Current list of filters
   * @param segments Current list of segments
   * @param min_len
   * @param j Index of the filter to erase
   * @param fj Filter to erase
   */
  void erase_filter(std::vector<Filter>& filters, std::vector<Segment>& segments, int min_len, int j, Filter& fj,
                    const SegDetParams& params)
  {
    if (fj.last_integration - fj.first - params.minimum_for_fusion > min_len)
      segments.emplace_back(fj, params.minimum_for_fusion);

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
  bool make_potential_fusion(std::vector<Filter>& filters, int index, std::vector<Segment>& segments, int min_len,
                             const SegDetParams& params)
  {
    auto f = filters[index];

    int  j                          = index + 1;
    bool current_filter_was_deleted = false;

    while (j < (int)filters.size())
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
   * @param discontinuity discontinuity allowed
   * @return true if the filter has to continue
   */
  bool filter_has_to_continue(Filter& f, int t, int discontinuity)
  {
    if (t - f.last_integration <= discontinuity)
      return true;

    if (f.currently_under_other != 0)
    {
      auto last_t = f.under_other[f.under_other.size() - 1].x;
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
  std::vector<Filter> filter_selection(std::vector<Filter>& filters, std::vector<Segment>& segments, int t,
                                       int two_matches, int min_len_embryo, const SegDetParams& params)
  {
    std::vector<Filter> filters_to_keep;

    int index = 0;
    while (index < (int)filters.size())
    {
      auto f = filters[index];

      // If slope of filter if greater than 50 degrees of lower than -50 degrees, stop it
      if (f.n_values.size() > 2 &&
          (f.current_slope > std::tan(50 * 3.14 / 180) || f.current_slope < -std::tan(50 * 3.14 / 180)))
      {
        index++;
        continue;
      }

      if (f.observation != std::nullopt)
      {
        if (two_matches > params.minimum_for_fusion &&
            make_potential_fusion(filters, index, segments, min_len_embryo, params))
          continue;

        integrate(f, t, params);
        compute_sigmas(f, params);

        filters_to_keep.push_back(f);
      }
      else if (filter_has_to_continue(f, t, params.discontinuity))
      {
        f.S = f.S_predicted;
        filters_to_keep.push_back(f);
      }
      else if (f.last_integration - f.first > min_len_embryo)
        segments.emplace_back(f, 0);

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
  void finish_traversal(std::vector<Filter>& filters, std::vector<Segment>& segments, int min_len,
                        const SegDetParams& params)
  {
    int i = 0;
    while (i < (int)filters.size())
    {
      auto& f = filters[i];
      if (make_potential_fusion(filters, i, segments, min_len, params))
        continue;

      if (f.last_integration - f.first > min_len)
        segments.emplace_back(f, 0);

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
   * @param params
   * @return
   */
  bool handle_find_filter(std::vector<Filter>& new_filters, std::vector<Filter*>& accepted,
                          const Eigen::Matrix<float, 3, 1>& obs, int t, const scribo::SegDetParams& params)
  {
    auto observation_s        = Observation();
    observation_s.obs         = obs;
    observation_s.match_count = static_cast<int>(accepted.size());
    observation_s.t           = t;

    for (auto& f : accepted)
    {
      auto obs_result = choose_nearest(*f, observation_s);

      if (obs_result != std::nullopt)
      {
        auto obs_result_value = obs_result.value();
        obs_result_value.match_count--;

        if (obs_result_value.match_count == 0)
          new_filters.emplace_back(t, obs_result_value.obs, params);
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


  /**
   *
   * @param image
   * @param min_len_embryo
   * @param discontinuity
   * @param params
   * @return
   */
  std::vector<Segment> traversal(const image2d<uint8_t>& image, int min_len_embryo, const SegDetParams& params)
  {
    int n_max = image.size(1), t_max = image.size(0);

    auto                filters  = std::vector<Filter>();  // List of current filters
    auto                segments = std::vector<Segment>(); // List of current segments
    std::vector<Filter> new_filters{};
    std::vector<Filter> selection{};

    int two_matches = 0; // Number of t where two segments matched the same observation
    // Useful to NOT check if filters has to be merged

    for (int t = 0; t < t_max; t++)
    {
      for (auto& filter : filters)
        predict(filter);

      new_filters.clear();
      bool two_matches_through_n = false;

      for (int n = 0; n < n_max; n++)
      {
        if (image({t, n}) < params.max_llum)
        {
          Eigen::Matrix<float, 3, 1> obs = determine_observation(image, n, t, n_max, params);

          std::vector<Filter*> accepted{}; // List of accepted filters by the current observation obs
          find_match(filters, accepted, obs, t, params);
          if (accepted.empty() && obs(1, 0) < params.max_thickness)
            new_filters.emplace_back(t, obs, params);
          else
            two_matches_through_n = handle_find_filter(new_filters, accepted, obs, t, params) || two_matches_through_n;
        }
      }

      if (two_matches_through_n)
        two_matches++;
      else
        two_matches = 0;

      // Selection for next turn
      selection = filter_selection(filters, segments, t, two_matches, min_len_embryo, params);
      // Merge selection and new_filters in filters
      update_current_filters(filters, selection, new_filters);
    }

    finish_traversal(filters, segments, min_len_embryo, params);

    return segments;
  }

  image2d<std::uint8_t> transpose(image2d<std::uint8_t> img)
  {
    image2d<std::uint8_t> transpose_image = image2d<std::uint8_t>(img.height(), img.width());
    mln::fill(transpose_image, 0);
    for (int x = 0; x < img.width(); ++x)
    {
      for (int y = 0; y < img.height(); ++y)
      {
        transpose_image({y, x}) = img({x, y});
      }
    }

    return transpose_image;
  }

  void transpose_segments(std::vector<Segment>& segments)
  {
    for (auto& segment : segments)
    {
      for (auto& span : segment.spans)
        std::swap(span.x, span.y);
      for (auto& span : segment.under_other_object)
        std::swap(span.x, span.y);

      std::swap(segment.last_span.x, segment.last_span.y);
      std::swap(segment.first_span.x, segment.first_span.y);
      segment.is_horizontal = false;
    }
  }

  /**
   * Compute the two traversals to detect horizontal and vertical segments
   * @param image image to extract segment from
   * @param min_len
   * @return Pair (horizontal segments,vertical segments)
   */
  std::pair<std::vector<Segment>, std::vector<Segment>> process(const image2d<std::uint8_t>& image, int min_len_embryo,
                                                                const SegDetParams& params)
  {
    // Horizontal traversal
    std::vector<Segment> horizontal_segments = traversal(image, min_len_embryo, params);

    // Vertical traversal
    std::vector<Segment> vertical_segments = traversal(transpose(image), min_len_embryo, params);

    transpose_segments(vertical_segments);
    return std::make_pair(horizontal_segments, vertical_segments);
  }
} // namespace scribo::internal