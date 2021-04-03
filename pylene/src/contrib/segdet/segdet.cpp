

#include "mln/contrib/segdet/segdet.hpp"
#include <mln/contrib/segdet/filter.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/io/imread.hpp>
#include <numeric>

#define SEGDET_SLOPE_MAX_VERTICAL 1.05
#define SEGDET_SLOPE_MAX_HORIZONTAL 1.0
#define SEGDET_MAX_LLUM 225
#define SEGDET_MAX_THK 100
#define SEGDET_RATIO_LUM 1

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
  std::vector<Segment> dothething(const image2d<uint8_t>& image, bool is_horizontal, uint min_len, uint discontinuity)
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
              auto obs_result = choose_nearest(*f, observation_s, t);
              // TODO
              (void) obs_result;
              (void) two_matches_through_n;
              (void) two_matches;
              (void) min_len;
              (void) discontinuity;
            }
          }
        }
      }
    }

    return std::vector<Segment>();
  }


  std::pair<std::vector<Segment>, std::vector<Segment>> process(const image2d<uint8_t>& image, uint min_len,
                                                                uint discontinuity)
  {
    // TODO Multi threading, splitter l'image
    std::vector<Segment> horizontal_segments = dothething(image, true, min_len, discontinuity);
    std::vector<Segment> vertical_segments   = dothething(image, false, min_len, discontinuity);

    return std::make_pair(horizontal_segments, vertical_segments);
  }

} // namespace mln::contrib::segdet