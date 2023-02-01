#include "../detect_line.hpp"

#include <mln/core/algorithm/fill.hpp>
#include <mln/core/image/ndbuffer_image.hpp>
#include <mln/core/image/ndimage.hpp>

#include <Eigen/Dense>
#include <algorithm>
#include <utility>

#include "bucket.hpp"
#include "extractors/extract_observation.hpp"
#include <mln/bp/transpose.hpp>

namespace scribo::internal
{
  static constexpr int   isolated_point  = 2;
  static constexpr int   slope_threshold = 10;
  static constexpr float slope_max       = 1.2f; // tan(50)

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
   * Compute the intersection of two spans
   * @param a
   * @param b
   * @return the intersection
   */
  inline Span intersect(const Eigen::Matrix<float, 3, 1>& a, const Eigen::Matrix<float, 3, 1>& b)
  {
    Span ret{};

    ret.thickness = 0;

    float amin = a(0) - a(1) / 2;
    float amax = a(0) + a(1) / 2;
    float bmin = b(0) - b(1) / 2;
    float bmax = b(0) + b(1) / 2;

    if (amin > bmax || bmin > amax)
      return ret;

    float min = std::max(amin, bmin);
    float max = std::min(amax, bmax);

    ret.y         = std::round((min + max) / 2.f);
    ret.thickness = std::round(max - min);

    return ret;
  }

  void find_match_bucket(Buckets& buckets, size_t bucket, std::vector<Tracker>& accepted,
                         const Eigen::Matrix<float, 3, 1>& obs, const int& t, int obs_thick, int obs_n_min,
                         int obs_n_max, const Descriptor& descriptor)
  {
    if (obs_thick < descriptor.max_thickness)
      buckets.remove_if(
          bucket,
          [=, &obs, &descriptor](const Tracker& f) { return f.impl->accepts(obs, obs_n_min, obs_n_max, descriptor); },
          accepted);

    buckets.for_each_tracker(bucket, [=](const Tracker& f) {
      if (f.impl->observation != std::nullopt)
        return;

      Span intersection = intersect(obs, f.impl->X_predicted);

      if (intersection.thickness <= 0)
        return;

      intersection.x = t;
      f.impl->under_other.push_back(intersection);
    });
  }

  /**
   * Compute list of trackers that accept the current Observation, add the tracker inside accepted list
   * @param trackers Current
   * @param obs Observation to match
   * @param t Current t
   */
  std::vector<Tracker> find_match(Buckets& buckets, const Eigen::Matrix<float, 3, 1>& obs, const int& t,
                                 float max_sigma_pos, const Descriptor& descriptor)
  {
    float max_sigma_thickD2 = std::max(max_sigma_pos, obs(1, 0) / 2.f);

    size_t obs_bucket_min =
        buckets.get_bucket_number(static_cast<size_t>(std::max(obs(0, 0) - max_sigma_thickD2, 0.f)));
    if (obs_bucket_min > 0)
      obs_bucket_min--;

    size_t obs_bucket_max = buckets.get_bucket_number(obs(0, 0) + max_sigma_thickD2);
    if (obs_bucket_max < buckets.get_bucket_count() - 1)
      obs_bucket_max++;

    float obs_thick    = obs(1, 0);
    float obs_thick_d2 = obs_thick / 2.f;
    int   obs_n_min    = std::floor(obs(0, 0) - obs_thick_d2);
    int   obs_n_max    = std::ceil(obs(0, 0) + obs_thick_d2);

    std::vector<Tracker> accepted;
    for (size_t b = obs_bucket_min; b <= obs_bucket_max; b++)
      find_match_bucket(buckets, b, accepted, obs, t, obs_thick, obs_n_min, obs_n_max, descriptor);

    return accepted;
  }

  /**
   * Check if the trackers are following the same observation
   * @param f first tracker
   * @param fj second tracker
   * @return true if the trackers are following the same observation for SEGDET_MINIMUM_FOR_FUSION length
   */
  bool need_fusion(const Tracker& fi, const Tracker& fj, const Descriptor& descriptor)
  {
    int i = 0;
    for (auto ite_i = fi.impl->same_observation.crbegin(), ite_j = fj.impl->same_observation.crbegin();
         i < descriptor.minimum_for_fusion && *ite_i == *ite_j; ite_i++, ite_j++)
      i++;
    return i == descriptor.minimum_for_fusion;
  }

  /**
   * Erase a tracker adding if needed the extract segment
   * @param trackers Current list of trackers
   * @param segments Current list of segments
   * @param j Index of the tracker to erase
   * @param descriptor parameters
   */
  void erase_tracker(std::vector<Tracker>& trackers, std::vector<Segment>& segments, size_t fi,
                    const Descriptor& descriptor)
  {
    if (trackers[fi].impl->last_integration - trackers[fi].impl->first - descriptor.minimum_for_fusion >
        descriptor.min_length_embryo)
      segments.emplace_back(std::move(trackers[fi]), descriptor.minimum_for_fusion);

    std::iter_swap(trackers.begin() + fi, trackers.begin() + trackers.size() - 1);
    trackers.pop_back();
  }

  /**
   * Check if a fusion with the current tracker is to be done
   * @param trackers Current list of tracker
   * @param index Index of the current tracker
   * @param segments Current list of segment
   * @return true if happened
   */
  bool make_potential_fusion(std::vector<Tracker>& trackers, size_t fi, std::vector<Segment>& segments,
                             const Descriptor& descriptor)
  {
    size_t fj                         = fi + 1;
    bool   current_tracker_was_deleted = false;

    while (fj < trackers.size())
    {
      if (trackers[fj].impl->same_observation.size() > static_cast<size_t>(descriptor.minimum_for_fusion) &&
          trackers[fj].impl->observation != std::nullopt && need_fusion(trackers[fi], trackers[fj], descriptor))
      {
        if (trackers[fi].impl->first < trackers[fj].impl->first)
        {
          erase_tracker(trackers, segments, fj, descriptor);
          continue;
        }
        else
        {
          current_tracker_was_deleted = true;
          erase_tracker(trackers, segments, fi, descriptor);
          break;
        }
      }

      fj++;
    }

    return current_tracker_was_deleted;
  }

  /**
   * Say if the tracker has to continue
   * @param f tracker to check
   * @param t current t
   * @param discontinuity discontinuity allowed
   * @return true if the tracker has to continue
   */
  bool tracker_has_to_continue(const Tracker& f, int t, const Descriptor& descriptor)
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
   * Integrate observation for trackers that matched an observation
   * Select trackers that have to continue
   * Add segment in segments if needed
   * @param trackers Current trackers
   * @param segments Current segments
   * @param t Current t
   * @param discontinuity
   * @return List of trackers that have to continue
   */
  std::vector<Tracker> tracker_selection(std::vector<Tracker>& trackers, std::vector<Segment>& segments, int t,
                                       const Descriptor& descriptor)
  {
    std::vector<Tracker> trackers_to_keep;

    size_t fi = 0;
    while (fi < trackers.size())
    {
      Tracker&& f = std::move(trackers[fi]);

      // Early stoping on high slopes
      if (f.impl->n_values.size() > slope_threshold && std::abs(f.impl->current_slope) > slope_max)
      {
        fi++;
        continue;
      }

      if (f.impl->observation != std::nullopt)
      {
        if (f.impl->same_observation.size() >= static_cast<size_t>(descriptor.minimum_for_fusion) &&
            make_potential_fusion(trackers, fi, segments, descriptor))
          continue;

        f.integrate(t, descriptor);

        trackers_to_keep.push_back(std::move(f));
      }
      else if (tracker_has_to_continue(f, t, descriptor))
      {
        trackers_to_keep.push_back(std::move(f));
      }
      else if (f.impl->last_integration - f.impl->first > descriptor.min_length_embryo)
        segments.emplace_back(std::move(f), 0);

      fi++;
    }

    return trackers_to_keep;
  }

  /**
   * Add last segment from current trackers if needed
   * @param trackers Current trackers
   * @param segments Current segments
   */
  void finish_traversal(std::vector<Tracker>& trackers, std::vector<Segment>& segments, const Descriptor& descriptor)
  {
    size_t fi = 0;
    while (fi < trackers.size())
    {
      if (trackers[fi].impl->last_integration - trackers[fi].impl->first > descriptor.min_length_embryo)
        segments.emplace_back(std::move(trackers[fi]), 0);

      fi++;
    }
  }

  /**
   * Handle case where one or more tracker matched the observation
   * @param new_trackers List of new trackers
   * @param accepted List of trackers that accepts the observation
   * @param obs
   * @param t
   * @param is_horizontal
   * @param descriptor
   * @return
   */
  void handle_find_tracker(Buckets& buckets, std::vector<Tracker>& accepted, std::vector<Tracker>& new_trackers,
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
          new_trackers.emplace_back(t, obs_result_value.obs, descriptor);
      }

      buckets.insert(std::move(f));
    }
  }

  /**
   * Merge selection and new_trackers in trackers
   * @param trackers
   * @param selection
   * @param new_trackers
   */
  void get_active_trackers(std::vector<Tracker>& trackers, std::vector<Tracker>& kept, std::vector<Tracker>& news)
  {
    trackers.clear();

    trackers.insert(trackers.begin(), std::move_iterator(kept.begin()), std::move_iterator(kept.end()));
    trackers.insert(trackers.begin(), std::move_iterator(news.begin()), std::move_iterator(news.end()));
  }

  float make_predictions(std::vector<Tracker>& trackers)
  {
    float max_dist = 0;
    for (auto& tracker : trackers)
    {
      tracker.predict();
      max_dist = std::max(max_dist, tracker.impl->sigma_position);
    }
    return max_dist;
  }

  std::vector<Tracker> match_observations_to_predictions(std::vector<Eigen::Matrix<float, 3, 1>>& observations,
                                                        Buckets& buckets, int t, float max_sigma_pos,
                                                        const Descriptor& descriptor)
  {
    std::vector<Tracker> new_trackers;

    size_t id = 0;
    for (const auto& obs : observations)
    {
      std::vector<Tracker> accepted = find_match(buckets, obs, t, max_sigma_pos, descriptor);
      if (accepted.empty() && obs(1, 0) < descriptor.max_thickness)
        new_trackers.emplace_back(t, obs, descriptor);
      else
        handle_find_tracker(buckets, accepted, new_trackers, obs, t, id++, descriptor);
    }

    return new_trackers;
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

    Buckets buckets(n_max, descriptor.bucket_size);

    std::vector<Segment>                    segments; // List of current segments
    std::vector<Tracker>                     trackers;
    std::vector<Tracker>                     new_trackers;
    std::vector<Tracker>                     tracker_kept;
    std::vector<Eigen::Matrix<float, 3, 1>> observations;

    for (int t = 0; t < t_max; t++)
    {
      float max_dist = make_predictions(trackers);

      observations = extract_observations(image, t, n_max, descriptor);

      buckets.acquire(trackers);
      new_trackers = match_observations_to_predictions(observations, buckets, t, max_dist, descriptor);
      buckets.release(trackers);

      tracker_kept = tracker_selection(trackers, segments, t, descriptor);

      // i_f = 0;
      get_active_trackers(trackers, tracker_kept, new_trackers);
    }

    finish_traversal(trackers, segments, descriptor);

    return segments;
  }

  image2d<std::uint8_t> transpose(image2d<std::uint8_t> img)
  {
    auto out = image2d<std::uint8_t>(img.height(), img.width());
    mln::bp::transpose(img.buffer(), out.buffer(), out.width(), out.height(), img.byte_stride(), out.byte_stride());
    return out;
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