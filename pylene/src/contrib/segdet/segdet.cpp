

#include "mln/contrib/segdet/segdet.hpp"
#include "mln/contrib/segdet/segment.hpp"
#include <mln/contrib/segdet/filter.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/io/imread.hpp>
#include <numeric>

#define SEGDET_SLOPE_MAX_VERTICAL 1.05
#define SEGDET_SLOPE_MAX_HORIZONTAL 1.0
#define SEGDET_MAX_LLUM 225
#define SEGDET_MAX_THK 100
#define SEGDET_RATIO_LUM 1
#define SEGDET_MAX_SLOPES_TOO_LARGE 5

#define SEGDET_MINIMUM_FOR_FUSION 15

namespace mln::contrib::segdet
{
  int detect_line(const mln::image2d<uint8_t>& image, uint min_len, uint discontinuity)
  {
    // TODO faire le top hat
    auto p = process(image, min_len, discontinuity);

    return 0;
  }

  uint8_t image_at(const image2d<uint8_t>& image, int n, int t, bool is_horizontal)
  {
    if (is_horizontal)
    {
      return image.buffer()[n * image.size(1) + t];
    }
    else
    {
      return image.buffer()[t * image.size(1) + n];
    }
  }

  Eigen::Matrix<double, 3, 1> determine_observation(const image2d<uint8_t>& image, uint32_t& n, uint32_t t,
                                                    uint32_t n_max, bool is_horizontal)
  {
    uint32_t thickness = 0;

    uint32_t n_max_lum = n;

    std::vector<uint8_t> luminosities_list;
    uint32_t             lumi;

    while (n + thickness < n_max && (lumi = image_at(image, t, n + thickness, is_horizontal)) < SEGDET_MAX_LLUM)
    {
      luminosities_list.push_back(lumi);

      if (lumi < luminosities_list[n_max_lum])
        n_max_lum = thickness;

      thickness += 1;
    }

    uint32_t max_lum = luminosities_list[n_max_lum];

    uint32_t m_lum = max_lum + (SEGDET_MAX_LLUM - max_lum) * SEGDET_RATIO_LUM;
    uint32_t n_end = n + thickness;

    if (n_end == n_max)
      n_end--;

    uint32_t n_to_skip = n_end;

    while (luminosities_list[n] > m_lum)
      n += 1;


    while (luminosities_list[n_end] > m_lum)
      n_end--;

    thickness         = n_end - n;
    uint32_t position = n + thickness / 2;


    const double mean =
        thickness == 0
            ? 0
            : std::accumulate(luminosities_list.begin() + n, luminosities_list.end() - n_end + n, 0.0) / thickness;

    n = n_to_skip;
    return Eigen::Matrix<double, 3, 1>(position, thickness, mean);
  }

  bool in_between(uint32_t min, uint32_t value, uint32_t max) { return min <= value && value < max; }

  bool find_match(std::vector<Filter>& filters, std::vector<Filter*>& accepted, const Eigen::Matrix<double, 3, 1>& obs,
                  const uint32_t& t, bool horizontal, uint32_t& index)
  {
    bool did_match = false;

    uint32_t obs_n_min = obs(0, 0) - obs(1, 0) / 2 - 1;
    uint32_t obs_n_max = obs(0, 0) + obs(1, 0) / 2 + 1;

    uint32_t n         = obs_n_min;
    uint32_t alpha     = n + obs(1, 0);
    uint32_t obs_thick = obs(1, 0);

    uint32_t i = index;

    while (i < filters.size() && filters[i].X_predicted(0, 0) - alpha < 10)
    {
      Filter& f = filters[i];
      if (accepts(f, obs, obs_n_min, obs_n_max))
      {
        accepted.push_back(&f);
        did_match = true;
      }
      else if (f.observation == std::nullopt && in_between(obs_n_min, f.n_min, obs_n_max) &&
               in_between(obs_n_min, f.n_max, obs_n_max) && f.X_predicted(1, 0) < obs_thick)
      {
        if (horizontal)
          f.currently_under_other.emplace_back(t, round(f.X_predicted(0, 0)), round(f.X_predicted(1, 0)));
        else
          f.currently_under_other.emplace_back(round(f.X_predicted(0, 0)), t, round(f.X_predicted(1, 0)));
      }
      i++;
    }

    if (i == filters.size())
      i--;

    while (i > 0 && filters[i].X_predicted(0, 0) - n > 0)
      i--;

    index = i;

    return did_match;
  }

  bool same_observation(const Filter& f, const Filter& fj)
  {
    if (f.n_values.size() < SEGDET_MINIMUM_FOR_FUSION || fj.n_values.size() < SEGDET_MINIMUM_FOR_FUSION)
      return false;
    for (uint32_t i = 0; i < SEGDET_MINIMUM_FOR_FUSION; i++)
    {
      auto k = f.thicknesses.size() - 1 - i;
      if (f.thicknesses[k] != fj.thicknesses[k] || f.t_values[k] != fj.t_values[k] || f.n_values[k] != fj.n_values[k])
        return false;
    }
    return true;
  }


  Segment make_segment_from_filter(Filter& filter, uint32_t min_len, uint32_t nb_to_remove)
  {
    uint32_t last_index = filter.n_values.size() - nb_to_remove;

    for (uint32_t i = 0; i < last_index; i++)
    {
      filter.segment_points.emplace_back(filter.n_values[i], filter.t_values[i], filter.thicknesses[i],
                                         filter.is_horizontal);
    }
    auto& point       = filter.segment_points;
    auto& under_other = filter.under_other;

    for (auto& p : filter.currently_under_other)
      under_other.push_back(p);

    auto first_part_slope =
        std::nullopt == filter.first_slope ? filter.slopes[min_len - 1] : filter.first_slope.value();
    auto last_part_slope = filter.slopes[last_index - 1];

    return Segment(point, under_other, first_part_slope, last_part_slope, filter.is_horizontal);
  }

  void erase_segments(std::vector<Filter>& filters, std::vector<Segment>& segments, uint32_t min_len, uint32_t j,
                      Filter& fj)
  {
    if (fj.last_integration - fj.first - SEGDET_MINIMUM_FOR_FUSION > min_len)
      segments.push_back(make_segment_from_filter(fj, min_len, SEGDET_MINIMUM_FOR_FUSION));
    filters.erase(filters.begin() + j);
  }


  bool make_potential_fusion(std::vector<Filter>& filters, uint32_t index, std::vector<Segment>& segments,
                             uint32_t min_len)
  {
    uint32_t j                          = index + 1;
    bool     current_filter_was_deleted = false;

    auto f = filters[index];
    while (j < filters.size())
    {
      auto fj = filters[j];
      if (fj.observation != std::nullopt && same_observation(f, fj))
      {
        if (f.first < fj.first)
        {
          erase_segments(filters, segments, min_len, j, fj);
        }
        else
        {
          current_filter_was_deleted = true;
          erase_segments(filters, segments, min_len, index, f);
          break;
        }
      }
      j++;
    }
    return current_filter_was_deleted;
  }


  void insert_in_sorted_filter_vector(Filter& f, std::vector<Filter>& filters)
  {
    auto elm = filters.begin();
    while (elm != filters.end() && (elm)->n_values[(elm)->n_values.size() - 1] < f.n_values[f.n_values.size() - 1])
    {
      elm++;
    }
    filters.insert(elm, std::move(f));
  }

  bool filter_has_to_continue(Filter& f, uint32_t t, uint32_t discontinuity)
  {
    if (t - f.last_integration < discontinuity)
    {
      return true;
    }
    if (!f.currently_under_other.empty())
    {
      auto last_t = f.is_horizontal ? f.currently_under_other[f.currently_under_other.size() - 1].x
                                    : f.currently_under_other[f.currently_under_other.size() - 1].y;
      return t - last_t < discontinuity;
    }
    return false;
  }

  std::vector<Filter> filter_selection(std::vector<Filter>& filters, std::vector<Segment>& segments, uint32_t t,
                                       uint32_t two_matches, uint32_t min_len, uint32_t discontinuity)
  {
    std::vector<Filter> filters_to_keep;

    size_t i = 0;
    while (i < filters.size())
    {
      auto f = filters[i];
      if (f.observation != std::nullopt)
      {
        if (two_matches > SEGDET_MINIMUM_FOR_FUSION && make_potential_fusion(filters, i, segments, min_len))
          continue;

        integrate(f, t);

        if (f.nb_current_slopes_over_slope_max > SEGDET_MAX_SLOPES_TOO_LARGE)
        {
          if (f.last_integration - f.first - SEGDET_MAX_SLOPES_TOO_LARGE > min_len)
            segments.push_back(make_segment_from_filter(f, min_len, 0));
          i++;
          continue;
        }

        insert_in_sorted_filter_vector(f, filters_to_keep);
      }
      else if (filter_has_to_continue(f, t, discontinuity))
      {
        f.S = f.S_predicted;
        insert_in_sorted_filter_vector(f, filters_to_keep);
      }
      else if (f.last_integration - f.first > min_len)
        segments.push_back(make_segment_from_filter(f, min_len, 0));
      i++;
    }
    return filters_to_keep;
  }


  void to_thrash(std::vector<Filter>& filters, std::vector<Segment>& segments, uint32_t min_len)
  {
    uint32_t i = 0;
    while (i < filters.size())
    {
      auto &f = filters[i];
      if (make_potential_fusion(filters, i, segments, min_len))
        continue;

      if (f.last_integration - f.first > min_len)
        segments.push_back(make_segment_from_filter(f, min_len, 0));

      i++;
    }
  }

  std::vector<Segment> traversal(const image2d<uint8_t>& image, bool is_horizontal, uint min_len, uint discontinuity)
  {
    uint32_t xmult     = 1;
    uint32_t ymult     = 0;
    double   slope_max = SEGDET_SLOPE_MAX_VERTICAL;
    if (is_horizontal)
    {
      xmult     = 0;
      ymult     = 1;
      slope_max = SEGDET_SLOPE_MAX_HORIZONTAL;
    }

    uint32_t n_max = image.size() * ymult + image.size(1) * xmult;
    uint32_t t_max = image.size() * xmult + image.size(1) * ymult;

    auto filters  = std::vector<Filter>();
    auto segments = std::vector<Segment>();

    uint32_t two_matches = 0;

    for (uint32_t t = 0; t < t_max; t++)
    {
      for (auto& filter : filters)
      {
        predict(filter);
      }

      std::vector<Filter> new_filters{};
      bool                two_matches_through_n = false;
      uint32_t            filter_index          = 0;

      for (uint32_t n = 0; n < n_max; n++)
      {
        if (image_at(image, n, t, is_horizontal) < SEGDET_MAX_LLUM)
        {
          Eigen::Matrix<double, 3, 1> obs = determine_observation(image, n, t, n_max, is_horizontal);

          std::vector<Filter*> accepted{};

          bool found = find_match(filters, accepted, obs, t, is_horizontal, filter_index);

          if (!found && obs(1, 0) < SEGDET_MAX_THK)
            new_filters.emplace_back(is_horizontal, t, slope_max, obs);
          else
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
                {
                  auto new_filter = Filter(is_horizontal, t, slope_max, obs_result_value.obs);
                  insert_in_sorted_filter_vector(new_filter, new_filters);

                  auto elm = new_filters.begin();
                  while (elm != new_filters.end() &&
                         elm->n_values[elm->n_values.size() - 1] < obs_result_value.obs(0, 0))
                  {
                    elm++;
                  }
                  new_filters.insert(elm, Filter(is_horizontal, t, slope_max, obs_result_value.obs));
                }
              }
            } // for
            if (observation_s.match_count > 1)
              two_matches_through_n = true;
          } // else
        }
      }

      if (two_matches_through_n)
        two_matches++;
      else
        two_matches = 0;


      auto selection = filter_selection(filters, segments, t, two_matches, min_len, discontinuity);

      filters.clear();
      filters.reserve(selection.size() + new_filters.size());
      std::merge(selection.begin(), selection.end(), new_filters.begin(), new_filters.end(), filters.begin(),
                 [](const Filter& lhs, const Filter& rhs) {
                   return lhs.n_values[lhs.n_values.size() - 1] < rhs.n_values[rhs.n_values.size() - 1];
                 });
    }

    to_thrash(filters, segments, min_len);

    return segments;
  }


  std::pair<std::vector<Segment>, std::vector<Segment>> process(const image2d<uint8_t>& image, uint min_len,
                                                                uint discontinuity)
  {
    // TODO Multi threading, splitter l'image
    std::vector<Segment> horizontal_segments = traversal(image, true, min_len, discontinuity);
    std::vector<Segment> vertical_segments   = traversal(image, false, min_len, discontinuity);

    return std::make_pair(horizontal_segments, vertical_segments);
  }

} // namespace mln::contrib::segdet