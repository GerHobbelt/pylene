#include "../detect_line.hpp"

#include <mln/core/algorithm/fill.hpp>
#include <mln/core/image/ndbuffer_image.hpp>
#include <mln/core/image/ndimage.hpp>

#include <Eigen/Dense>
#include <algorithm>
#include <utility>

#include "extractors/extract_observation.hpp"

namespace scribo::internal
{
  static constexpr int   isolated_point  = 2;
  static constexpr int   slope_threshold = 10;
  static constexpr float slope_max       = 1.2f; // tan(50)

  struct Buckets
  {
    const size_t bucket_size;
    const size_t bucket_count;

    std::vector<std::vector<Filter>> container;

    Buckets(size_t n_max, const Descriptor& descriptor)
      : bucket_size(std::min(static_cast<size_t>(descriptor.bucket_size), n_max))
      , bucket_count(n_max / bucket_size + (n_max % bucket_size == 0 ? 0 : 1))
    {
      container = std::vector<std::vector<Filter>>();
      for (size_t i = 0; i < bucket_count; i++)
        container.push_back(std::vector<Filter>());
    }

    size_t get_bucket_number(size_t n)
    {
      return std::max(static_cast<size_t>(0), std::min(bucket_count - 1, n / bucket_size));
    }

    size_t get_bucket_number(Filter& f) { return get_bucket_number(f.get_position()); }

    void insert(Filter&& filter) { container[get_bucket_number(filter)].push_back(std::move(filter)); }

    void fill(std::vector<Filter>& filters)
    {
      for (auto& filter : filters)
        insert(std::move(filter));
      filters.clear();
    }

    void empty(std::vector<Filter>& filters)
    {
      for (size_t i = 0; i < bucket_count; i++)
      {
        for (size_t j = 0; j < container[i].size(); j++)
          filters.push_back(std::move(container[i][j]));
        container[i].clear();
      }
    }

    std::vector<Filter>& get_bucket(size_t i) { return container[i]; }
  };

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

  void find_match_bucket(Buckets& buckets, size_t bucket, std::vector<Filter>& accepted,
                         const Eigen::Matrix<float, 3, 1>& obs, const int& t, int obs_thick, int obs_n_min,
                         int obs_n_max, const Descriptor& descriptor)
  {

    for (size_t i = 0; i < buckets.container[bucket].size(); i++)
    {
      Filter&& f = std::move(buckets.container[bucket][i]);

      if (obs_thick < descriptor.max_thickness && f.impl->accepts(obs, obs_n_min, obs_n_max, descriptor))
      {
        accepted.push_back(std::move(f));

        std::iter_swap(buckets.container[bucket].begin() + i,
                       buckets.container[bucket].begin() + buckets.container[bucket].size() - 1);
        buckets.container[bucket].pop_back();
      }
      else
      {
        if (f.impl->observation == std::nullopt && in_between(obs_n_min - 1, f.impl->n_min, obs_n_max + 1) &&
            in_between(obs_n_min - 1, f.impl->n_max, obs_n_max + 1) && f.impl->X_predicted(1, 0) < obs_thick)
        {
          Span span{};
          span.x         = t;
          span.y         = round(f.impl->X_predicted(0, 0));
          span.thickness = round(f.impl->X_predicted(1, 0));
          f.impl->under_other.push_back(span);
        }

        buckets.container[bucket][i] = std::move(f);
      }
    }
  }

  /**
   * Compute list of filters that accept the current Observation, add the filter inside accepted list
   * @param filters Current
   * @param obs Observation to match
   * @param t Current t
   */
  std::vector<Filter> find_match(Buckets& buckets, const Eigen::Matrix<float, 3, 1>& obs, const int& t,
                                 const Descriptor& descriptor)
  {
    int obs_thick    = obs(1, 0);
    int obs_thick_d2 = obs_thick / 2;

    int obs_n_min = obs(0, 0) - obs_thick_d2;
    if (obs_n_min != 0)
      obs_n_min--;
    int obs_n_max = obs(0, 0) + obs_thick_d2 + 1;

    size_t obs_bucket_min = buckets.get_bucket_number(static_cast<size_t>(obs_n_min));
    size_t obs_bucket_max = buckets.get_bucket_number(static_cast<size_t>(obs_n_max));

    if (obs_bucket_min > 0)
      obs_bucket_min--;
    if (obs_bucket_max + 1 < buckets.bucket_count)
      obs_bucket_max++;

    std::vector<Filter> accepted{};
    for (size_t b = obs_bucket_min; b <= obs_bucket_max; b++)
      find_match_bucket(buckets, b, accepted, obs, t, obs_thick, obs_n_min, obs_n_max, descriptor);

    return accepted;
  }

  /**
   * Check if the filters are following the same observation
   * @param f first filter
   * @param fj second filter
   * @return true if the filters are following the same observation for SEGDET_MINIMUM_FOR_FUSION length
   */
  bool need_fusion(const Filter& fi, const Filter& fj, const Descriptor& descriptor)
  {
    int i = 0;
    for (auto ite_i = fi.impl->same_observation.crbegin(), ite_j = fj.impl->same_observation.crbegin();
         i < descriptor.minimum_for_fusion && *ite_i == *ite_j; ite_i++, ite_j++)
      i++;
    return i == descriptor.minimum_for_fusion;
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
      if (filters[fj].impl->same_observation.size() > static_cast<size_t>(descriptor.minimum_for_fusion) &&
          filters[fj].impl->observation != std::nullopt && need_fusion(filters[fi], filters[fj], descriptor))
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
   * @param discontinuity
   * @return List of filters that have to continue
   */
  std::vector<Filter> filter_selection(std::vector<Filter>& filters, std::vector<Segment>& segments, int t,
                                       const Descriptor& descriptor)
  {
    std::vector<Filter> filters_to_keep;

    size_t fi = 0;
    while (fi < filters.size())
    {
      Filter&& f = std::move(filters[fi]);

      // Early stoping on high slopes
      if (f.impl->n_values.size() > slope_threshold && std::abs(f.impl->current_slope) > slope_max)
      {
        fi++;
        continue;
      }

      if (f.impl->observation != std::nullopt)
      {
        if (f.impl->same_observation.size() >= static_cast<size_t>(descriptor.minimum_for_fusion) &&
            make_potential_fusion(filters, fi, segments, descriptor))
          continue;

        f.integrate(t, descriptor);

        filters_to_keep.push_back(std::move(f));
      }
      else if (filter_has_to_continue(f, t, descriptor))
      {
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
  void handle_find_filter(Buckets& buckets, std::vector<Filter>& accepted, std::vector<Filter>& new_filters,
                          const Eigen::Matrix<float, 3, 1>& obs, int t, size_t id, const Descriptor& descriptor)
  {
    auto observation_s        = Observation();
    observation_s.obs         = obs;
    observation_s.match_count = static_cast<int>(accepted.size());
    observation_s.t           = t;
    observation_s.id          = id;

    for (auto& f : accepted)
    {
      auto obs_result = f.impl->choose_nearest(observation_s);

      if (obs_result != std::nullopt)
      {
        auto obs_result_value = obs_result.value();
        obs_result_value.match_count--;

        if (obs_result_value.match_count == 0)
          new_filters.emplace_back(t, obs_result_value.obs, descriptor);
      }

      buckets.insert(std::move(f));
    }
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
    for (auto& filter : filters)
      filter.predict();
  }

  std::vector<Filter> match_observations_to_predictions(std::vector<Eigen::Matrix<float, 3, 1>>& observations,
                                                        Buckets& buckets, int t, const Descriptor& descriptor)
  {
    std::vector<Filter> new_filters{};

    size_t id = 0;
    for (const auto& obs : observations)
    {
      std::vector<Filter> accepted = find_match(buckets, obs, t, descriptor);
      if (accepted.empty() && obs(1, 0) < descriptor.max_thickness)
        new_filters.emplace_back(t, obs, descriptor);
      else
        handle_find_filter(buckets, accepted, new_filters, obs, t, id++, descriptor);
    }

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

    Buckets buckets(n_max, descriptor);

    std::vector<Segment>                    segments; // List of current segments
    std::vector<Filter>                     filters;
    std::vector<Filter>                     new_filters;
    std::vector<Filter>                     filter_kept;
    std::vector<Eigen::Matrix<float, 3, 1>> observations;

    for (int t = 0; t < t_max; t++)
    {
      make_predictions(filters);

      observations = extract_observations(image, t, n_max, descriptor);

      buckets.fill(filters);
      new_filters = match_observations_to_predictions(observations, buckets, t, descriptor);
      buckets.empty(filters);

      filter_kept = filter_selection(filters, segments, t, descriptor);

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