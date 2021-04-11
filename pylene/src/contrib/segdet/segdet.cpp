

#include "mln/contrib/segdet/segdet.hpp"
#include "mln/contrib/segdet/segment.hpp"
#include <mln/contrib/segdet/filter.hpp>
#include <mln/contrib/segdet/linearregression.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/transform.hpp>
#include <mln/core/image/views.hpp>
#include <mln/io/imprint.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>

#include <numeric>
#include <utility>

#define SEGDET_SLOPE_MAX_VERTICAL 1.05
#define SEGDET_SLOPE_MAX_HORIZONTAL 1.0
#define SEGDET_MAX_LLUM 225
#define SEGDET_MAX_THK 100
#define SEGDET_RATIO_LUM 1
#define SEGDET_MERGE_SLOPE_VARIATION 0.4
#define SEGDET_MERGE_DISTANCE_MAX 8
#define SEGDET_MAX_SLOPES_TOO_LARGE 5
#define SEGDET_THRESHOLD_INTERSECTION 0.8

#define SEGDET_MINIMUM_FOR_FUSION 15

namespace mln::contrib::segdet
{

  uint8_t image_at(const image2d<uint8_t>& image, int n, int t, bool is_horizontal)
  {
    if (is_horizontal)
      return image.at({t, n});
    else
      return image.at({n, t});
  }

  Eigen::Matrix<double, 3, 1> determine_observation(const image2d<uint8_t>& image, uint32_t& n, uint32_t t,
                                                    uint32_t n_max, bool is_horizontal)
  {
    uint32_t thickness = 0;

    uint32_t n_max_lum = 0;

    std::vector<uint8_t> luminosities_list = std::vector<uint8_t>();
    uint32_t             lumi;

    while (n + thickness < n_max && (lumi = image_at(image, n + thickness, t, is_horizontal)) < SEGDET_MAX_LLUM)
    {
      luminosities_list.push_back(lumi);

      if (lumi < luminosities_list[n_max_lum])
        n_max_lum = thickness;

      thickness += 1;
    }

    uint32_t max_lum = luminosities_list[n_max_lum];

    uint32_t m_lum   = max_lum + (SEGDET_MAX_LLUM - max_lum) * SEGDET_RATIO_LUM;
    auto     n_start = n;
    uint32_t n_end   = n + thickness;

    if (n_end == n_max)
      n_end--;

    uint32_t n_to_skip = n_end;

    while (luminosities_list[n - n_start] > m_lum)
      n += 1;

    int k = luminosities_list.size() - 1;
    while (luminosities_list[k] > m_lum)
      k--;

    n_end = n_start + k;
    n_end++;

    thickness         = n_end - n;
    uint32_t position = n + thickness / 2;

    const double mean = mln::contrib::segdet::mean(luminosities_list, n - n_start, n_end - n_start);

    n = n_to_skip;
    return Eigen::Matrix<double, 3, 1>(position, thickness, mean);
  }

  bool in_between(uint32_t min, uint32_t value, uint32_t max) { return min <= value && value < max; }

  bool find_match(std::vector<std::shared_ptr<Filter>>& filters, std::vector<Filter*>& accepted,
                  const Eigen::Matrix<double, 3, 1>& obs, const uint32_t& t, bool horizontal, uint32_t& index)
  {
    bool did_match = false;

    uint32_t obs_n_min = obs(0, 0) - obs(1, 0) / 2 - 1;
    uint32_t obs_n_max = obs(0, 0) + obs(1, 0) / 2 + 1;

    uint32_t n         = obs_n_min;
    uint32_t alpha     = n + obs(1, 0);
    uint32_t obs_thick = obs(1, 0);

    uint32_t i = index;

    while (i < filters.size() && filters[i]->X_predicted(0, 0) - alpha < 10)
    {
      Filter& f = *filters[i];
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

    if (i == filters.size() && filters.size())
      i--;

    while (i > 0 && filters[i]->X_predicted(0, 0) - n > 0)
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

  void erase_segments(std::vector<std::shared_ptr<Filter>>& filters, std::vector<Segment>& segments, uint32_t min_len,
                      uint32_t j, Filter& fj)
  {
    if (fj.last_integration - fj.first - SEGDET_MINIMUM_FOR_FUSION > min_len)
      segments.push_back(make_segment_from_filter(fj, min_len, SEGDET_MINIMUM_FOR_FUSION));
    filters.erase(filters.begin() + j);
  }


  bool make_potential_fusion(std::vector<std::shared_ptr<Filter>>& filters, uint32_t index,
                             std::vector<Segment>& segments, uint32_t min_len)
  {
    uint32_t j                          = index + 1;
    bool     current_filter_was_deleted = false;

    auto f = filters[index];
    while (j < filters.size())
    {
      auto fj = filters[j];
      if (fj->observation != std::nullopt && same_observation(*f, *fj))
      {
        if (f->first < fj->first)
        {
          erase_segments(filters, segments, min_len, j, *fj);
        }
        else
        {
          current_filter_was_deleted = true;
          erase_segments(filters, segments, min_len, index, *f);
          break;
        }
      }
      j++;
    }
    return current_filter_was_deleted;
  }


  void insert_in_sorted_filter_vector(std::shared_ptr<Filter>& f, std::vector<std::shared_ptr<Filter>>& filters)
  {
    auto elm = filters.begin();
    while (elm != filters.end() && (*elm)->n_values[(*elm)->n_values.size() - 1] < f->n_values[f->n_values.size() - 1])
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
      return t - last_t <= discontinuity;
    }
    return false;
  }

  std::vector<std::shared_ptr<Filter>> filter_selection(std::vector<std::shared_ptr<Filter>>& filters,
                                                        std::vector<Segment>& segments, uint32_t t,
                                                        uint32_t two_matches, uint32_t min_len, uint32_t discontinuity)
  {
    std::vector<std::shared_ptr<Filter>> filters_to_keep;

    size_t i = 0;
    while (i < filters.size())
    {
      auto f = filters[i];
      if (f->observation != std::nullopt)
      {
        if (two_matches > SEGDET_MINIMUM_FOR_FUSION && make_potential_fusion(filters, i, segments, min_len))
          continue;

        integrate(*f, t);

        if (f->nb_current_slopes_over_slope_max > SEGDET_MAX_SLOPES_TOO_LARGE)
        {
          if (f->last_integration - f->first - SEGDET_MAX_SLOPES_TOO_LARGE > min_len)
            segments.push_back(make_segment_from_filter(*f, min_len, 0));
          i++;
          continue;
        }

        insert_in_sorted_filter_vector(f, filters_to_keep);
      }
      else if (filter_has_to_continue(*f, t, discontinuity))
      {
        f->S = f->S_predicted;
        insert_in_sorted_filter_vector(f, filters_to_keep);
      }
      else if (f->last_integration - f->first > min_len)
        segments.push_back(make_segment_from_filter(*f, min_len, 0));
      i++;
    }
    return filters_to_keep;
  }


  void to_thrash(std::vector<std::shared_ptr<Filter>>& filters, std::vector<Segment>& segments, uint32_t min_len)
  {
    uint32_t i = 0;
    while (i < filters.size())
    {
      auto& f = filters[i];
      if (make_potential_fusion(filters, i, segments, min_len))
        continue;

      if (f->last_integration - f->first > min_len)
        segments.push_back(make_segment_from_filter(*f, min_len, 0));

      i++;
    }
  }


  template <typename T, typename F>
  void merge(const std::vector<T>& arr1, const std::vector<T>& arr2, std::vector<T>& arr_out, F cmp)
  {
    size_t i = 0;
    size_t j = 0;
    while (i < arr1.size() && j < arr2.size())
    {
      if (cmp(arr1[i], arr2[j]))
        arr_out.push_back(arr1[i++]);
      else
        arr_out.push_back(arr2[j++]);
    }
    while (i < arr1.size())
      arr_out.push_back(arr1[i++]);
    while (j < arr2.size())
      arr_out.push_back(arr2[j++]);
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

    uint32_t n_max = image.size() * xmult + image.size(1) * ymult;
    uint32_t t_max = image.size() * ymult + image.size(1) * xmult;

    auto filters  = std::vector<std::shared_ptr<Filter>>();
    auto segments = std::vector<Segment>();

    uint32_t two_matches = 0;

    for (uint32_t t = 0; t < t_max; t++)
    {
      for (auto& filter : filters)
      {
        predict(*filter);
      }

      std::vector<std::shared_ptr<Filter>> new_filters{};
      bool                                 two_matches_through_n = false;
      uint32_t                             filter_index          = 0;

      for (uint32_t n = 0; n < n_max; n++)
      {
        if (image_at(image, n, t, is_horizontal) < SEGDET_MAX_LLUM)
        {
          Eigen::Matrix<double, 3, 1> obs = determine_observation(image, n, t, n_max, is_horizontal);

          std::vector<Filter*> accepted{};

          bool found = find_match(filters, accepted, obs, t, is_horizontal, filter_index);

          if (!found && obs(1, 0) < SEGDET_MAX_THK)
            new_filters.push_back(std::make_shared<Filter>(is_horizontal, t, slope_max, obs));
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
                  auto new_filter = std::make_shared<Filter>(is_horizontal, t, slope_max, obs_result_value.obs);
                  insert_in_sorted_filter_vector(new_filter, new_filters);

                  auto elm = new_filters.begin();
                  while (elm != new_filters.end() &&
                         (*elm)->n_values[(*elm)->n_values.size() - 1] < obs_result_value.obs(0, 0))
                  {
                    elm++;
                  }
                  new_filters.insert(elm, std::make_shared<Filter>(is_horizontal, t, slope_max, obs_result_value.obs));
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
      merge(selection, new_filters, filters,
            [](const std::shared_ptr<Filter>& lhs, const std::shared_ptr<Filter>& rhs) {
              return lhs->n_values[lhs->n_values.size() - 1] < rhs->n_values[rhs->n_values.size() - 1];
            });
    }

    to_thrash(filters, segments, min_len);

    return segments;
  }

  uint32_t distance_points(const Point& p1, const Point& p2)
  {
    auto xvar = p1.x - p2.x;
    auto yvar = p1.y - p2.y;

    xvar *= xvar;
    yvar *= yvar;

    return std::sqrt(xvar + yvar);
  }

  bool has_to_link(Segment a, Segment b)
  {
    if (std::abs(a.last_part_slope - b.first_part_slope) > SEGDET_MERGE_SLOPE_VARIATION)
      return false;

    if (a.points.size() && a.points.size() &&
        distance_points(a.points[a.points.size() - 1], b.points[0]) < SEGDET_MERGE_DISTANCE_MAX)
      return true;

    return false;
  }

  template <typename T>
  void add_all(std::vector<T>& lhs, const std::vector<T>& rhs)
  {
    for (auto elm : rhs)
      lhs.push_back(elm);
  }

  void merge_segments(Segment& a, const Segment& b)
  {
    if (a.is_horizontal)
      a.length = b.points[b.points.size() - 1].x - a.points[0].x + 1;
    else
      a.length = b.points[b.points.size() - 1].y - a.points[0].y + 1;

    add_all(a.under_other, b.under_other);
    add_all(a.points, b.points);

    a.nb_pixels += b.nb_pixels;

    a.last_part_slope = b.last_part_slope;
  }

  void segment_linking(std::vector<Segment>& segments)
  {
    size_t i = 0;
    while (i < segments.size())
    {
      size_t j = i + 1;
      while (j < segments.size())
      {
        if (has_to_link(segments[i], segments[j]))
        {
          merge_segments(segments[i], segments[j]);
          segments.erase(segments.begin() + j);
        }
        else
          j++;
      }
      i++;
    }
  }


  void draw_labeled_pixel(image2d<uint16_t>& img, uint16_t label, int x, int y)
  {
    if (img({x, y}) != 0)
      img({x, y}) = 1;
    else
      img({x, y}) = label;
  }


  void draw_labeled_point(image2d<uint16_t>& img, uint16_t label, Point point, bool is_horizontal)
  {
    auto thickness = point.thickness / 2;
    auto is_odd    = point.thickness % 2;

    if (is_horizontal)
    {
      for (int i = -thickness; i < static_cast<int>(thickness) + static_cast<int>(is_odd); i++)
      {
        if (static_cast<int>(point.y) + i < img.size(1))
          draw_labeled_pixel(img, label, static_cast<int>(point.x), static_cast<int>(point.y + i));
      }
    }
    else
    {
      for (int i = -thickness; i < static_cast<int>(thickness) + static_cast<int>(is_odd); i++)
      {
        if (static_cast<int>(point.x) + i < img.size(0))
          draw_labeled_pixel(img, label, static_cast<int>(point.x + i), static_cast<int>(point.y));
      }
    }
  }

  enum labeling_type
  {
    LABELING_TYPE_VERTICAL,
    LABELING_TYPE_HORIZONTAL,
  };

  image2d<uint16_t> labeled_arr(image2d<uint16_t> out, size_t width, size_t height,
                                std::vector<Segment>& horizontal_segments, std::vector<Segment>& vertical_segments,
                                labeling_type type, bool include_other)
  {
    (void)(width);
    (void)(height);
    auto                 arr      = out;
    std::vector<Segment> segments = type == LABELING_TYPE_HORIZONTAL ? horizontal_segments : vertical_segments;

    for (size_t i = 0; i < segments.size(); i++)
    {
      auto segment = segments[i];
      for (auto& point : segment.points)
        draw_labeled_point(arr, i + 3, point, segment.is_horizontal);
      if (include_other)
        for (auto& point : segment.under_other)
          draw_labeled_point(arr, 1, point, segment.is_horizontal);
    }
    return arr;
  }

  void binarize_img(image2d<uint16_t> img)
  {
    mln_foreach (auto p, img.domain())
    {
      if (img(p) != 0)
        img(p) = 1;
      else
        img(p) = 0;
    }
  }

  void intersect(image2d<uint16_t> img, image2d<uint16_t> img2, image2d<uint16_t> out)
  {
    mln_foreach (auto pt, img.domain())
      out(pt) = img(pt) * img2(pt);
  }

  void remove_dup(std::vector<Segment>& segments_to_compare, std::vector<Segment>& segments_removable, size_t width,
                  size_t height)
  {
    image2d<uint16_t> first_output = image2d<uint16_t>(width, height);

    mln_foreach (auto pt, first_output.domain())
      first_output(pt) = 0;

    mln::io::imsave(first_output, "first_output.pgm");
    labeled_arr(first_output, width, height, segments_to_compare, segments_removable, LABELING_TYPE_HORIZONTAL, true);

    image2d<uint16_t> second_output = image2d<uint16_t>(width, height);

    mln_foreach (auto pt, second_output.domain())
      second_output(pt) = 0;

    mln::io::imsave(second_output, "second_output.pgm");
    labeled_arr(second_output, width, height, segments_to_compare, segments_removable, LABELING_TYPE_VERTICAL, true);

    mln::io::imsave(first_output, "first_output.pgm");
    mln::io::imsave(second_output, "second_output.pgm");

    //    auto second_output_bin = mln::view::transform(second_output, [](uint16_t p) { return (p != 0) ? 1 : 0; });

    binarize_img(second_output);

    //    auto second_output_bin_c = image2d<uint16_t>(second_output_bin);
    mln::io::imsave(second_output, "second_output_bin.pgm");

    image2d<uint16_t> intersection = image2d<uint16_t>(width, height);

    //    auto intersection =
    //        mln::view::transform(first_output, second_output_bin, [](uint16_t x, uint16_t y) { return x * y; });
    intersect(first_output, second_output, intersection);

    mln::io::imsave(intersection, "intersection.pgm");

    std::vector<uint16_t> segments = std::vector<uint16_t>(segments_removable.size());

    mln_foreach (auto p, intersection.values())
    {
      if (p >= 3)
        segments[p - 3]++;
    }

    int k = 0;
    for (size_t i = 0; i < segments.size(); i++)
    {
      double segments_ratio = 0;
      if (segments_removable[i - k].nb_pixels != 0)
        segments_ratio = segments[i] / (double)segments_removable[i - k].nb_pixels;
      if (segments_removable[i - k].nb_pixels == 0 || segments_ratio > SEGDET_THRESHOLD_INTERSECTION)
        segments_removable.erase(segments_removable.begin() + i - k);
      k++;
    }
  }

  void remove_duplicates(std::pair<std::vector<Segment>, std::vector<Segment>>& pair, size_t img_width,
                         size_t img_height)
  {
    remove_dup(pair.first, pair.second, img_width, img_height);
    remove_dup(pair.second, pair.first, img_width, img_height);
  }


  void post_process(std::pair<std::vector<Segment>, std::vector<Segment>>& pair, size_t img_width, size_t img_height)
  {
    segment_linking(pair.first);
    segment_linking(pair.second);
    remove_duplicates(pair, img_width, img_height);
  }


  std::pair<std::vector<Segment>, std::vector<Segment>> process(const image2d<uint8_t>& image, uint min_len,
                                                                uint discontinuity)
  {
    // TODO Multi threading, splitter l'image
    std::vector<Segment> horizontal_segments = traversal(image, true, min_len, discontinuity);
    std::vector<Segment> vertical_segments   = traversal(image, false, min_len, discontinuity);

    return std::make_pair(horizontal_segments, vertical_segments);
  }

  int detect_line(const mln::image2d<uint8_t>& image, uint min_len, uint discontinuity)
  {
    // TODO faire le top hat
    auto p = process(image, min_len, discontinuity);

    post_process(p, image.size(0), image.size(1));

    return 0;
  }

} // namespace mln::contrib::segdet