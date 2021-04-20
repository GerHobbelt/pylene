
#include <Eigen/Dense>
#include <mln/contrib/segdet/filter.hpp>
#include <mln/contrib/segdet/linearregression.hpp>
#include <mln/contrib/segdet/segdet.hpp>
#include <mln/contrib/segdet/segment.hpp>
#include <mln/core/algorithm/fill.hpp>
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
  /**
   * Give the value of the pixel in (n, t) according to traversal direction
   * @param image Image to take pixel from
   * @param n
   * @param t
   * @param is_horizontal
   * @return the value of the pixel
   */
  uint8_t image_at(image2d<uint8_t> image, int n, int t, bool is_horizontal)
  {
    // TODO remove the check done by image.at(,) using image(,)
    return is_horizontal ? image.at({t, n}) : image.at({n, t});
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
                                                    uint32_t n_max, bool is_horizontal)
  {
    uint32_t thickness = 0;
    uint32_t n_max_lum = 0;

    std::vector<uint8_t> luminosities_list = std::vector<uint8_t>();
    uint32_t             lumi;

    // n + thickess: current position in the n-th line
    while (n + thickness < n_max && (lumi = image_at(image, n + thickness, t, is_horizontal)) < SEGDET_MAX_LLUM)
    {
      luminosities_list.push_back(lumi);

      if (lumi < luminosities_list[n_max_lum])
        n_max_lum = thickness;

      thickness += 1;
    }

    uint32_t n_to_skip = n + thickness;                // Position of the next n to work on
    uint32_t max_lum   = luminosities_list[n_max_lum]; // Max luminosity of the current span

    // m_lum : max_luminosity of what is accepted in the span
    uint32_t m_lum   = max_lum + (SEGDET_MAX_LLUM - max_lum) * SEGDET_RATIO_LUM;
    auto     n_start = n;             // n_start is AT LEAST n
    uint32_t n_end   = n + thickness; // n_end is AT MOST n + thickness

    if (n_end == n_max) // In case we stopped because of outOfBound value
      n_end--;

    while (luminosities_list[n - n_start] > m_lum)
      n += 1;

    uint32_t k = luminosities_list.size() - 1;
    while (luminosities_list[k] > m_lum)
      k--;
    n_end = n_start + k + 1; // Plus one because the last pixel has to be included

    thickness         = n_end - n;
    uint32_t position = n + thickness / 2;

    const double mean = mln::contrib::segdet::mean(luminosities_list, n - n_start, n_end - n_start);

    n = n_to_skip; // Setting reference value of n

    return Eigen::Matrix<double, 3, 1>(position, thickness, mean);
  }

  /**
   * Say if a value is betwen two other
   * @param min
   * @param value
   * @param max
   * @return true if the value is between
   */
  inline bool in_between(uint32_t min, uint32_t value, uint32_t max) { return min <= value && value < max; }

  /**
   * Add a point in the under other attribute of filter
   * @param f Filter in which add the point
   * @param t
   * @param n
   * @param thickness
   */
  void add_point_under_other(Filter& f, uint32_t t, uint32_t n, uint32_t thickness)
  {
    if (f.is_horizontal)
      f.currently_under_other.emplace_back(t, n, thickness);
    else
      f.currently_under_other.emplace_back(n, t, thickness);
  }

  /**
   * Compute list of filters that accept the current Observation, add the filter inside accepted list
   * @param filters Current
   * @param obs Observation to match
   * @param t Current t
   * @param index Current index in n column
   */
  void find_match(std::vector<Filter>& filters, std::vector<Filter*>& accepted, const Eigen::Matrix<double, 3, 1>& obs,
                  const uint32_t& t, uint32_t& index)
  {
    uint32_t obs_n_min = obs(0, 0) - obs(1, 0) / 2;
    if (obs_n_min != 0)
      obs_n_min--;
    uint32_t obs_n_max = obs(0, 0) + obs(1, 0) / 2 + 1;
    uint32_t obs_thick = obs(1, 0);

    // Only checking the acceptation for near predictions
    while (index < filters.size() && filters[index].X_predicted(0, 0) - obs_n_max < 10)
    {
      Filter& f = filters[index];
      if (accepts(f, obs, obs_n_min, obs_n_max))
        accepted.push_back(&f);
      else if (f.observation == std::nullopt && in_between(obs_n_min, f.n_min, obs_n_max) &&
               in_between(obs_n_min, f.n_max, obs_n_max) && f.X_predicted(1, 0) < obs_thick)
      {
        add_point_under_other(f, t, round(f.X_predicted(0, 0)), round(f.X_predicted(1, 0)));
      }
      index++;
    }

    // Check the overflow
    if (index == filters.size() && !filters.empty())
      index--;

    // Go back with index
    while (index > 0 && filters[index].X_predicted(0, 0) - obs_n_max > 0)
      index--;
  }

  /**
   * Check if the filters are following the same observation
   * @param f first filter
   * @param fj second filter
   * @return true if the filters are following the same observation for SEGDET_MINIMUM_FOR_FUSION length
   */
  bool same_observation(const Filter& f, const Filter& fj)
  {
    if (f.n_values.size() < SEGDET_MINIMUM_FOR_FUSION || fj.n_values.size() < SEGDET_MINIMUM_FOR_FUSION)
      return false;

    for (uint32_t i = 0; i < SEGDET_MINIMUM_FOR_FUSION; i++)
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

  /**
   * Erase a filter adding if needed the extract segment
   * @param filters Current list of filters
   * @param segments Current list of segments
   * @param min_len
   * @param j Index of the filter to erase
   * @param fj Filter to erase
   */
  void erase_filter(std::vector<Filter>& filters, std::vector<Segment>& segments, uint32_t min_len, uint32_t j,
                    Filter& fj)
  {
    if (fj.last_integration - fj.first - SEGDET_MINIMUM_FOR_FUSION > min_len)
      segments.push_back(make_segment_from_filter(fj, min_len, SEGDET_MINIMUM_FOR_FUSION));

    filters.erase(filters.begin() + j);
  }

  /**
   * Check if a fusion with the current filter
   * @param filters Current list of filter
   * @param index Index of the current filter
   * @param segments Current list of segment
   * @param min_len
   * @return true if happened
   */
  bool make_potential_fusion(std::vector<Filter>& filters, uint32_t index, std::vector<Segment>& segments,
                             uint32_t min_len)
  {
    auto f = filters[index];

    uint32_t j                          = index + 1;
    bool     current_filter_was_deleted = false;

    while (j < filters.size())
    {
      auto fj = filters[j];
      if (fj.observation != std::nullopt && same_observation(f, fj))
      {
        if (f.first < fj.first)
          erase_filter(filters, segments, min_len, j, fj);
        else
        {
          current_filter_was_deleted = true;
          erase_filter(filters, segments, min_len, index, f);
          break;
        }
      }

      j++;
    }

    return current_filter_was_deleted;
  }


  /**
   * Insert f in filters sorted in increasing order following the position of the prediction
   * @param f
   * @param filters
   */
  void insert_in_sorted_filter_vector(Filter& f, std::vector<Filter>& filters)
  {
    auto elm = filters.begin();

    while (elm != filters.end() && (*elm).n_values[(*elm).n_values.size() - 1] < f.n_values[f.n_values.size() - 1])
      elm++;

    filters.insert(elm, std::move(f));
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
      auto last_t = f.is_horizontal ? f.currently_under_other[f.currently_under_other.size() - 1].x
                                    : f.currently_under_other[f.currently_under_other.size() - 1].y;
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
   * @param min_len
   * @param discontinuity
   * @return List of filters that have to continue
   */
  std::vector<Filter> filter_selection(std::vector<Filter>& filters, std::vector<Segment>& segments, uint32_t t,
                                       uint32_t two_matches, uint32_t min_len, uint32_t discontinuity)
  {
    std::vector<Filter> filters_to_keep;

    size_t index = 0;
    while (index < filters.size())
    {
      auto f = filters[index];

      if (f.observation != std::nullopt)
      {
        if (two_matches > SEGDET_MINIMUM_FOR_FUSION && make_potential_fusion(filters, index, segments, min_len))
          continue;

        integrate(f, t);
        compute_sigmas(f);

        if (f.nb_current_slopes_over_slope_max > SEGDET_MAX_SLOPES_TOO_LARGE)
        {
          if (f.last_integration - f.first - SEGDET_MAX_SLOPES_TOO_LARGE > min_len)
            segments.push_back(make_segment_from_filter(f, min_len, 0));
          index++;
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
  void to_thrash(std::vector<Filter>& filters, std::vector<Segment>& segments, uint32_t min_len)
  {
    uint32_t i = 0;
    while (i < filters.size())
    {
      auto& f = filters[i];
      if (make_potential_fusion(filters, i, segments, min_len))
        continue;

      if (f.last_integration - f.first > min_len)
        segments.push_back(make_segment_from_filter(f, min_len, 0));

      i++;
    }
  }

  /**
   *
   * @tparam T
   * @tparam F
   * @param arr1
   * @param arr2
   * @param arr_out
   * @param cmp
   */
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

  /**
   * Set the parameters of the traversal according to the direction
   * @param is_horizontal direction
   * @param xmult
   * @param ymult
   * @param slope_max
   * @param n_max
   * @param t_max
   * @param width Width of the image
   * @param height Height of the image
   */
  void set_parameters(bool is_horizontal, uint32_t& xmult, uint32_t& ymult, double& slope_max, uint32_t& n_max,
                      uint32_t& t_max, uint32_t width, uint32_t height)
  {
    if (is_horizontal)
    {
      xmult     = 0;
      ymult     = 1;
      slope_max = SEGDET_SLOPE_MAX_HORIZONTAL;
      n_max     = height;
      t_max     = width;
    }
    else
    {
      xmult     = 1;
      ymult     = 0;
      slope_max = SEGDET_SLOPE_MAX_VERTICAL;
      n_max     = width;
      t_max     = height;
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
                          const Eigen::Matrix<double, 3, 1>& obs, uint32_t& t, bool is_horizontal, double slope_max)
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

          auto elm = new_filters.begin();
          while (elm != new_filters.end() && (*elm).n_values[(*elm).n_values.size() - 1] < obs_result_value.obs(0, 0))
          {
            elm++;
          }
          new_filters.insert(elm, Filter(is_horizontal, t, slope_max, obs_result_value.obs));
        }
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
    filters.reserve(selection.size() + new_filters.size());
    merge(selection, new_filters, filters, [](const Filter& lhs, const Filter& rhs) {
      return lhs.n_values[lhs.n_values.size() - 1] < rhs.n_values[rhs.n_values.size() - 1];
    });
  }

  std::vector<Segment> traversal(const image2d<uint8_t>& image, bool is_horizontal, uint min_len, uint discontinuity)
  {
    // Usefull parameter used in the function
    uint32_t xmult, ymult;
    double   slope_max;
    uint32_t n_max, t_max;

    set_parameters(is_horizontal, xmult, ymult, slope_max, n_max, t_max, image.size(), image.size(1));

    auto filters  = std::vector<Filter>();  // List of current filters
    auto segments = std::vector<Segment>(); // List of current segments

    uint32_t two_matches = 0; // Number of t where two segments matched the same observation
    // Useful to NOT check if filters has to be merged

    for (uint32_t t = 0; t < t_max; t++)
    {
      for (auto& filter : filters)
        predict(filter);

      std::vector<Filter> new_filters{};
      bool                two_matches_through_n = false;
      uint32_t            filter_index          = 0;

      for (uint32_t n = 0; n < n_max; n++)
      {
        if (image_at(image, n, t, is_horizontal) < SEGDET_MAX_LLUM)
        {
          Eigen::Matrix<double, 3, 1> obs = determine_observation(image, n, t, n_max, is_horizontal);

          std::vector<Filter*> accepted{}; // List of accepted filters by the current observation obs
          find_match(filters, accepted, obs, t, filter_index);
          if (accepted.empty() && obs(1, 0) < SEGDET_MAX_THK)
            new_filters.emplace_back(is_horizontal, t, slope_max, obs);
          else
            two_matches_through_n =
                handle_find_filter(new_filters, accepted, obs, t, is_horizontal, slope_max) || two_matches_through_n;
        }
      }

      if (two_matches_through_n)
        two_matches++;
      else
        two_matches = 0;

      auto selection = filter_selection(filters, segments, t, two_matches, min_len, discontinuity);
      update_current_filters(filters, selection, new_filters);
    }

    to_thrash(filters, segments, min_len);

    return segments;
  }

  /**
   * Compute the distance between 2 points
   * @param p1
   * @param p2
   * @return
   */
  uint32_t distance_points(const Point& p1, const Point& p2)
  {
    auto xvar = p1.x - p2.x;
    auto yvar = p1.y - p2.y;

    xvar *= xvar;
    yvar *= yvar;

    return std::sqrt(xvar + yvar);
  }

  /**
   * Say if 2 segments have to be linked
   * @param a
   * @param b
   * @return
   */
  bool has_to_link(Segment a, Segment b)
  {
    if (std::abs(a.last_part_slope - b.first_part_slope) > SEGDET_MERGE_SLOPE_VARIATION)
      return false;

    if (a.points.size() && a.points.size() &&
        distance_points(a.points[a.points.size() - 1], b.points[0]) < SEGDET_MERGE_DISTANCE_MAX)
      return true;

    return false;
  }

  /**
   * Push all elements in right in left vector
   * @tparam T
   * @param lhs
   * @param rhs
   */
  template <typename T>
  void add_all(std::vector<T>& lhs, const std::vector<T>& rhs)
  {
    for (auto elm : rhs)
      lhs.push_back(elm);
  }

  /**
   * Merge the second segment to the first
   * @param a First segment
   * @param b Second segment
   */
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

  /**
   * Call the function linking segments if needed
   * @param segments All segments
   */
  void segment_link(std::vector<Segment>& segments)
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

  /**
   * Call segment link for horizontal and vertical segments
   * @param pair First is horizontal segment and second vertical segments
   */
  void segment_linking(std::pair<std::vector<Segment>, std::vector<Segment>>& pair)
  {
    segment_link(pair.first);
    segment_link(pair.second);
  }

  /**
   * Label a pixel in img
   * @param img Image to labelize
   * @param label Color
   * @param x X position
   * @param y Y position
   */
  void draw_labeled_pixel(image2d<uint16_t> img, uint16_t label, int x, int y)
  {
    if (img({x, y}) != 0)
      img({x, y}) = 1;
    else
      img({x, y}) = label;
  }

  /**
   * Draw a pint in img
   * @param img Image to labelize
   * @param label Color
   * @param point Point to draw
   * @param is_horizontal
   */
  void draw_labeled_point(image2d<uint16_t> img, uint16_t label, Point point, bool is_horizontal)
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

  /**
   * Draw segments in img out according to type
   * @param out Image to draw in
   * @param horizontal_segments
   * @param vertical_segments
   * @param type Labeling type
   * @param include_other true if need to include under other points
   */
  void labeled_arr(image2d<uint16_t> out, std::vector<Segment>& horizontal_segments,
                   std::vector<Segment>& vertical_segments, labeling_type type, bool include_other)
  {
    std::vector<Segment> segments = type == LABELING_TYPE_HORIZONTAL ? horizontal_segments : vertical_segments;

    for (size_t i = 0; i < segments.size(); i++)
    {
      auto segment = segments[i];
      for (auto& point : segment.points)
        draw_labeled_point(out, i + 3, point, segment.is_horizontal);
      if (include_other)
        for (auto& point : segment.under_other)
          draw_labeled_point(out, 1, point, segment.is_horizontal);
    }
  }

  /**
   * Binarize the image
   * @param img Image to binarize
   */
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

  /**
   * Compute the intersection between two images and store it in out
   * @param img first image
   * @param img2 second image
   * @param out image storing result
   */
  void intersect(image2d<uint16_t> img, image2d<uint16_t> img2, image2d<uint16_t> out)
  {
    mln_foreach (auto pt, img.domain())
      out(pt) = img(pt) * img2(pt);
  }

  /**
   * Remove duplication of segment
   * @param segments_to_compare
   * @param segments_removable
   * @param width
   * @param height
   */
  void remove_dup(std::vector<Segment>& segments_to_compare, std::vector<Segment>& segments_removable, size_t width,
                  size_t height)
  {
    image2d<uint16_t> first_output = image2d<uint16_t>(width, height);

    mln::fill(first_output, 0);

    labeled_arr(first_output, segments_to_compare, segments_removable, LABELING_TYPE_HORIZONTAL, true);

    image2d<uint16_t> second_output = image2d<uint16_t>(width, height);

    mln::fill(second_output, 0);

    labeled_arr(second_output, segments_to_compare, segments_removable, LABELING_TYPE_VERTICAL, true);

    auto second_output_bin =
        mln::view::transform(second_output, [](uint16_t p) -> uint8_t { return (p != 0) ? 1 : 0; });

    binarize_img(first_output);

    image2d<uint16_t> intersection = image2d<uint16_t>(width, height);
    mln::fill(intersection, 0);

    //    auto intersection =
    //        mln::view::transform(first_output, second_output_bin, [](uint16_t x, uint16_t y) { return x * y; });
    intersect(first_output, second_output, intersection);

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


  /**
   * Call remove duplicates for horizontal and vertical segments
   * @param pair Pair (horizontal segments,vertical segments)
   * @param img_width Width of the image where segments were extract
   * @param img_height Height of the image where segments were extract
   */
  void remove_duplicates(std::pair<std::vector<Segment>, std::vector<Segment>>& pair, size_t img_width,
                         size_t img_height)
  {
    remove_dup(pair.first, pair.second, img_width, img_height);
    remove_dup(pair.second, pair.first, img_width, img_height);
  }

  /**
   * Post process segments linking them and removing duplications
   * @param pair Pair (horizontal segments,vertical segments)
   * @param img_width Width of the image where segments were extract
   * @param img_height Height of the image where segments were extract
   */
  void post_process(std::pair<std::vector<Segment>, std::vector<Segment>>& pair, size_t img_width, size_t img_height)
  {
    segment_linking(pair);
    remove_duplicates(pair, img_width, img_height);
  }

  /**
   * Compute the two traversals to detect horizontal and vertical segments
   * @param image image to extract segment from
   * @param min_len
   * @param discontinuity
   * @return Pair (horizontal segments,vertical segments)
   */
  std::pair<std::vector<Segment>, std::vector<Segment>> process(const image2d<uint8_t>& image, uint min_len,
                                                                uint discontinuity)
  {
    // TODO Multi threading, splitter l'image
    std::vector<Segment> horizontal_segments = traversal(image, true, min_len, discontinuity);
    std::vector<Segment> vertical_segments   = traversal(image, false, min_len, discontinuity);

    return std::make_pair(horizontal_segments, vertical_segments);
  }

  // Public functions

  std::vector<Segment> detect_line(image2d<uint8_t> image, uint min_len, uint discontinuity)
  {
    // TODO faire le top hat
    auto p = process(image, min_len, discontinuity);

    post_process(p, image.size(0), image.size(1));

    std::vector<Segment> out;

    for (auto& seg : p.first)
      out.push_back(seg);
    for (auto& seg : p.second)
      out.push_back(seg);

    return out;
  }

} // namespace mln::contrib::segdet