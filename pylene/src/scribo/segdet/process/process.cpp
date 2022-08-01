#include "../detect_line.hpp"

#include <mln/core/algorithm/fill.hpp>
#include <mln/core/image/ndbuffer_image.hpp>
#include <mln/core/image/ndimage.hpp>

#include <Eigen/Dense>
#include <algorithm>
#include <utility>

namespace scribo::internal
{
  static constexpr int   isolated_point  = 2;
  static constexpr int   slope_threshold = 10;
  static constexpr float slope_max       = 1.2f; // tan(50)

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
  inline bool in_between(float min, float value, float max)
  {
    return min <= value && value < max;
  }


  /**
   * Compute list of filters that accept the current Observation, add the filter inside accepted list
   * @param filters Current
   * @param obs Observation to match
   * @param t Current t
   */
  std::vector<size_t> find_match(std::vector<Filter>& filters, const Eigen::Matrix<float, 3, 1>& obs, const int& t,
                                 const Descriptor& descriptor)
  {
    int obs_thick    = obs(1, 0);
    int obs_thick_d2 = obs_thick / 2;

    int obs_n_min = obs(0, 0) - obs_thick_d2;
    if (obs_n_min != 0)
      obs_n_min--;

    int obs_n_max = obs(0, 0) + obs_thick_d2 + 1;

    std::vector<size_t> accepted{};
    for (size_t f_index = 0; f_index < filters.size(); f_index++)
    {
      Filter&& f = std::move(filters[f_index]);

      if (obs_thick < descriptor.max_thickness && f.impl->accepts(obs, obs_n_min, obs_n_max, descriptor))
        accepted.push_back(f_index);
      else if (f.impl->observation == std::nullopt && in_between(obs_n_min - 1, f.impl->n_min, obs_n_max + 1) &&
               in_between(obs_n_min - 1, f.impl->n_max, obs_n_max + 1) && f.impl->X_predicted(1, 0) < obs_thick)
      {
        Span span{};
        span.x         = t;
        span.y         = round(f.impl->X_predicted(0, 0));
        span.thickness = round(f.impl->X_predicted(1, 0));
        f.impl->under_other.push_back(span);
      }

      filters[f_index] = std::move(f);
    }

    return accepted;
  }

  /**
   * Check if the filters are following the same observation
   * @param f first filter
   * @param fj second filter
   * @return true if the filters are following the same observation for SEGDET_MINIMUM_FOR_FUSION length
   */
  bool same_observation(const Filter& fi, const Filter& fj, const Descriptor& descriptor)
  {
    if (static_cast<int>(fi.impl->n_values.size()) < descriptor.minimum_for_fusion ||
        static_cast<int>(fj.impl->n_values.size()) < descriptor.minimum_for_fusion)
      return false;

    for (int i = 0; i < descriptor.minimum_for_fusion; i++)
    {
      int k  = static_cast<int>(fi.impl->thicknesses.size()) - 1 - i;
      int kj = static_cast<int>(fj.impl->thicknesses.size()) - 1 - i;

      if (fi.impl->thicknesses[k] != fj.impl->thicknesses[kj] || fi.impl->t_values[k] != fj.impl->t_values[kj] ||
          fi.impl->n_values[k] != fj.impl->n_values[kj])
        return false;
    }

    return true;
  }

  /**
   * Erase a filter adding if needed the extract segment
   * @param filters Current list of filters
   * @param segments Current list of segments
   * @param j Index of the filter to erase
   * @param descriptor parameters
   */
  void erase_filter(std::vector<Filter>& filters, std::vector<Segment>& segments, size_t fi,
                    const Descriptor& descriptor)
  {
    if (filters[fi].impl->last_integration - filters[fi].impl->first - descriptor.minimum_for_fusion >
        descriptor.min_length_embryo)
      segments.emplace_back(std::move(filters[fi]), descriptor.minimum_for_fusion);

    std::iter_swap(filters.begin() + fi, filters.begin() + filters.size() - 1);
    filters.pop_back();
  }

  /**
   * Check if a fusion with the current filter is to be done
   * @param filters Current list of filter
   * @param index Index of the current filter
   * @param segments Current list of segment
   * @return true if happened
   */
  bool make_potential_fusion(std::vector<Filter>& filters, size_t fi, std::vector<Segment>& segments,
                             const Descriptor& descriptor)
  {
    size_t fj                         = fi + 1;
    bool   current_filter_was_deleted = false;

    while (fj < filters.size())
    {
      if (filters[fj].impl->observation != std::nullopt && same_observation(filters[fi], filters[fj], descriptor))
      {
        if (filters[fi].impl->first < filters[fj].impl->first)
        {
          erase_filter(filters, segments, fj, descriptor);
          continue;
        }
        else
        {
          current_filter_was_deleted = true;
          erase_filter(filters, segments, fi, descriptor);
          break;
        }
      }

      fj++;
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
  bool filter_has_to_continue(const Filter& f, int t, const Descriptor& descriptor)
  {
    // Isolated point
    if (f.impl->last_integration - f.impl->first <= isolated_point)
      return false;

    // Discontinuity
    if (t - f.impl->last_integration <= (f.impl->last_integration - f.impl->first) * descriptor.discontinuity_relative +
                                            descriptor.discontinuity_absolute)
      return true;

    // Under an other object
    if (!f.impl->under_other.empty() && t == f.impl->under_other[f.impl->under_other.size() - 1].x)
      return true;

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
   * @param discontinuity
   * @return List of filters that have to continue
   */
  std::vector<Filter> filter_selection(std::vector<Filter>& filters, std::vector<Segment>& segments, int t,
                                       int two_matches, const Descriptor& descriptor)
  {
    std::vector<Filter> filters_to_keep;

    size_t fi = 0;
    while (fi < filters.size())
    {
      Filter&& f = std::move(filters[fi]);

      if (f.impl->n_values.size() > slope_threshold && std::abs(f.impl->current_slope) > slope_max)
      {
        fi++;
        continue;
      }

      if (f.impl->observation != std::nullopt)
      {
        if (two_matches > descriptor.minimum_for_fusion && make_potential_fusion(filters, fi, segments, descriptor))
          continue;

        f.integrate(t, descriptor);

        filters_to_keep.push_back(std::move(f));
      }
      else if (filter_has_to_continue(f, t, descriptor))
      {
        f.impl->S = f.impl->S_predicted;
        filters_to_keep.push_back(std::move(f));
      }
      else if (f.impl->last_integration - f.impl->first > descriptor.min_length_embryo)
        segments.emplace_back(std::move(f), 0);

      fi++;
    }

    return filters_to_keep;
  }

  /**
   * Add last segment from current filters if needed
   * @param filters Current filters
   * @param segments Current segments
   */
  void finish_traversal(std::vector<Filter>& filters, std::vector<Segment>& segments, const Descriptor& descriptor)
  {
    size_t fi = 0;
    while (fi < filters.size())
    {
      if (make_potential_fusion(filters, fi, segments, descriptor))
        continue;

      if (filters[fi].impl->last_integration - filters[fi].impl->first > descriptor.min_length_embryo)
        segments.emplace_back(std::move(filters[fi]), 0);

      fi++;
    }
  }

  /**
   * Handle case where one or more filter matched the observation
   * @param new_filters List of new filters
   * @param accepted List of filters that accepts the observation
   * @param obs
   * @param t
   * @param is_horizontal
   * @param descriptor
   * @return
   */
  bool handle_find_filter(std::vector<Filter>& current_filters, std::vector<size_t>& accepted,
                          std::vector<Filter>& new_filters, const Eigen::Matrix<float, 3, 1>& obs, int t,
                          const Descriptor& descriptor)
  {
    auto observation_s        = Observation();
    observation_s.obs         = obs;
    observation_s.match_count = static_cast<int>(accepted.size());
    observation_s.t           = t;

    for (auto& index : accepted)
    {
      auto obs_result = current_filters[index].impl->choose_nearest(observation_s);

      if (obs_result != std::nullopt)
      {
        auto obs_result_value = obs_result.value();
        obs_result_value.match_count--;

        if (obs_result_value.match_count == 0)
          new_filters.emplace_back(t, obs_result_value.obs, descriptor);
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
  std::vector<Filter> get_active_filters(std::vector<Filter>& kept, std::vector<Filter>& news)
  {
    std::vector<Filter> filters;

    filters.insert(filters.begin(), std::move_iterator(kept.begin()), std::move_iterator(kept.end()));
    filters.insert(filters.begin(), std::move_iterator(news.begin()), std::move_iterator(news.end()));

    return filters;
  }

  void make_predictions(std::vector<Filter>& filters)
  {
    for (Filter& filter : filters)
      filter.predict();
  }

  std::vector<Eigen::Matrix<float, 3, 1>> extract_observations(const image2d<uint8_t>& image, int t, int n_max,
                                                               const Descriptor& descriptor)
  {
    std::vector<Eigen::Matrix<float, 3, 1>> observations;
    for (int n = 0; n < n_max; n++)
      if (image({t, n}) < descriptor.max_llum)
        observations.push_back(determine_observation(image, n, t, n_max, descriptor));
    return observations;
  }

  std::vector<Filter> match_observations_to_predictions(std::vector<Eigen::Matrix<float, 3, 1>>& observations,
                                                        std::vector<Filter>& filters, int& two_matches, int t,
                                                        const Descriptor& descriptor)
  {
    std::vector<Filter> new_filters{};
    bool                two_matches_through_t = false;
    for (const auto& obs : observations)
    {
      std::vector<size_t> accepted = find_match(filters, obs, t, descriptor);
      if (accepted.empty() && obs(1, 0) < descriptor.max_thickness)
        new_filters.emplace_back(t, obs, descriptor);
      else
        two_matches_through_t =
            handle_find_filter(filters, accepted, new_filters, obs, t, descriptor) || two_matches_through_t;
    }

    if (two_matches_through_t)
      two_matches++;
    else
      two_matches = 0;

    return new_filters;
  }

  /**
   *
   * @param image
   * @param discontinuity
   * @param descriptor
   * @return
   */
  std::vector<Segment> traversal(const image2d<uint8_t>& image, const Descriptor& descriptor)
  {
    int n_max = image.size(1), t_max = image.size(0);

    auto                                    filters  = std::vector<Filter>();  // List of current filters
    auto                                    segments = std::vector<Segment>(); // List of current segments
    std::vector<Filter>                     new_filters{};
    std::vector<Filter>                     filter_kept{};
    std::vector<Eigen::Matrix<float, 3, 1>> observations{};

    // Useful to NOT check if filters has to be merged
    int two_matches = 0; // Number of t where two segments matched the same observation

    for (int t = 0; t < t_max; t++)
    {
      make_predictions(filters);

      observations = extract_observations(image, t, n_max, descriptor);

      new_filters = match_observations_to_predictions(observations, filters, two_matches, t, descriptor);
      filter_kept = filter_selection(filters, segments, t, two_matches, descriptor);

      filters = get_active_filters(filter_kept, new_filters);
    }

    finish_traversal(filters, segments, descriptor);

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
   * @return Pair (horizontal segments,vertical segments)
   */
  std::pair<std::vector<Segment>, std::vector<Segment>> process(const image2d<std::uint8_t>& image,
                                                                const Descriptor&            descriptor)
  {
    // Horizontal traversal
    std::vector<Segment> horizontal_segments;
    if (descriptor.traversal_mode != SEGDET_PROCESS_TRAVERSAL_MODE_ENUM::VERTICAL)
      horizontal_segments = traversal(image, descriptor);

    // Vertical traversal
    std::vector<Segment> vertical_segments;
    if (descriptor.traversal_mode != SEGDET_PROCESS_TRAVERSAL_MODE_ENUM::HORIZONTAL)
    {
      vertical_segments = traversal(transpose(image), descriptor);
      transpose_segments(vertical_segments);
    }

    return std::make_pair(horizontal_segments, vertical_segments);
  }
} // namespace scribo::internal