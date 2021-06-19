#include <mln/contrib/segdet/detect_line.hpp>

#include <algorithm>
#include <mln/contrib/segdet/segdet.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/image/view/transform.hpp>
#include <utility>

namespace mln::contrib::segdet
{
  /**
   * Compute the distance between 2 points
   * @param p1
   * @param p2
   * @return
   */
  double distance_points(const Point& p1, const Point& p2)
  {
    int xvar = (int)p1.x - (int)p2.x;
    int yvar = (int)p1.y - (int)p2.y;

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
  double distance_linking(Segment& a, const Segment& b, const Parameters& params)
  {
    if (std::abs(a.last_part_slope - b.first_part_slope) > params.merge_slope_variation)
      return params.merge_distance_max;

    return distance_points(a.last_point, b.first_point);
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

    for (auto& p : b.points)
      a.points.push_back(p);
    for (auto& p : b.under_other_object)
      a.under_other_object.push_back(p);

    a.nb_pixels += b.nb_pixels;

    a.last_part_slope = b.last_part_slope;
    a.last_point      = b.last_point;
  }

  /**
   * Call the function linking segments if needed
   * @param segments All segments
   */
  void segment_link(std::vector<Segment>& segments, Parameters params)
  {
    size_t i = 0;
    while (i < segments.size())
    {
      size_t j = i + 1;

      size_t best_index    = i;
      double best_distance = params.merge_distance_max;
      while (j < segments.size())
      {
        auto distance_link = distance_linking(segments[i], segments[j], params);
        if (distance_link < best_distance)
        {
          best_index    = j;
          best_distance = distance_link;
        }
        else
          j++;
      }

      if (best_distance < params.merge_distance_max)
      {
        merge_segments(segments[i], segments[best_index]);
        segments.erase(segments.begin() + best_index);
      }

      i++;
    }
  }

  /**
   * Call segment link for horizontal and vertical segments
   * @param pair First is horizontal segment and second vertical segments
   */
  void segment_linking(std::pair<std::vector<Segment>, std::vector<Segment>>& pair, Parameters params)
  {
    segment_link(pair.first, params);
    segment_link(pair.second, params);
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
    img({x, y}) = img({x, y}) != 0 ? 1 : label;
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
        int y = static_cast<int>(point.y) + i;
        if (0 <= y && y < img.size(1))
          draw_labeled_pixel(img, label, static_cast<int>(point.x), y);
      }
    }
    else
    {
      for (int i = -thickness; i < static_cast<int>(thickness) + static_cast<int>(is_odd); i++)
      {
        int x = static_cast<int>(point.x) + i;
        if (0 <= x && x < img.size(0))
          draw_labeled_pixel(img, label, x, static_cast<int>(point.y));
      }
    }
  }

  enum labeling_type
  {
    LABELING_TYPE_VERTICAL,
    LABELING_TYPE_HORIZONTAL,
  };


  void labeled_arr(image2d<uint16_t> out, const std::vector<Segment>& segments)
  {
    for (size_t i = 0; i < segments.size(); i++)
    {
      for (auto& point : segments[i].points)
        draw_labeled_point(out, i + 3, point, segments[i].is_horizontal);
      for (auto& point : segments[i].under_other_object)
        draw_labeled_point(out, i + 3, point, segments[i].is_horizontal);
    }
  }


  /**
   * Draw segments in img out according to type
   * @param out Image to draw in
   * @param horizontal_segments
   * @param vertical_segments
   * @param type Labeling type
   */
  void labeled_arr(image2d<uint16_t> out, std::vector<Segment>& horizontal_segments,
                   std::vector<Segment>& vertical_segments, labeling_type type)
  {
    std::vector<Segment> segments = type == LABELING_TYPE_HORIZONTAL ? horizontal_segments : vertical_segments;

    labeled_arr(out, segments);
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
                  size_t height, Parameters params)
  {
    image2d<uint16_t> first_output = image2d<uint16_t>(width, height);
    mln::fill(first_output, 0);
    labeled_arr(first_output, segments_to_compare, segments_removable, LABELING_TYPE_HORIZONTAL);

    image2d<uint16_t> second_output = image2d<uint16_t>(width, height);
    mln::fill(second_output, 0);
    labeled_arr(second_output, segments_to_compare, segments_removable, LABELING_TYPE_VERTICAL);

    auto second_output_bin =
        mln::view::transform(second_output, [](uint16_t p) -> uint8_t { return (p != 0) ? 1 : 0; });

    binarize_img(first_output);

    image2d<uint16_t> intersection = image2d<uint16_t>(width, height);
    mln::fill(intersection, 0);

    intersect(first_output, second_output, intersection);


    std::vector<uint16_t> segments = std::vector<uint16_t>(segments_removable.size());

    for (unsigned short& segment : segments)
      segment = 0;

    mln_foreach (auto v, intersection.values())
    {
      if (v >= 3)
        segments[v - 3]++;
    }

    int k = 0;
    for (size_t i = 0; i < segments.size(); i++)
    {
      double segments_ratio = 0;
      if (segments_removable[i - k].nb_pixels != 0)
        segments_ratio = segments[i] / (double)segments_removable[i - k].nb_pixels;
      if (segments_removable[i - k].nb_pixels == 0 || segments_ratio > params.threshold_intersection)
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
                         size_t img_height, Parameters params)
  {
    remove_dup(pair.first, pair.second, img_width, img_height, params);
    remove_dup(pair.second, pair.first, img_width, img_height, params);
  }

  /**
   * Only keep segments that are above a given length and combine pair
   * @param p Pair of vectors of segments
   * @param min_len The length
   * @return
   */
  std::vector<Segment> filter_length(std::pair<std::vector<Segment>, std::vector<Segment>>& p, uint min_len)
  {
    std::vector<Segment> res{};

    for (auto& seg : p.first)
    {
      if (seg.length > min_len)
        res.push_back(seg);
    }
    for (auto& seg : p.second)
    {
      if (seg.length > min_len)
        res.push_back(seg);
    }

    return res;
  }

  bool is_intersection(image2d<uint16_t> img, const Point &point, bool is_horizontal)
  {
    auto thickness = point.thickness / 2;
    int is_odd    = point.thickness % 2;

    if (is_horizontal)
    {
      for (int i = -thickness; i < static_cast<int>(thickness) + is_odd; i++)
      {
        int y = static_cast<int>(point.y) + i;
        if (0 <= y && y < img.size(1) && img({static_cast<int>(point.x), y}) == 2)
          return false;
      }
    }
    else
    {
      for (int i = -thickness; i < static_cast<int>(thickness) + is_odd; i++)
      {
        int x = static_cast<int>(point.x) + i;
        if (0 <= x && x < img.size(0) && img({x, static_cast<int>(point.y)}) == 2)
          return false;
      }
    }

    return true;
  }

  std::vector<Segment> retreive_good_under_other(std::vector<Segment> &segments, size_t width, size_t height)
  {
    image2d<uint16_t> image = image2d<uint16_t>(width, height);
    mln::fill(image, 0);

    for (const auto &segment : segments)
    {
      for (const auto &point : segment.under_other_object)
      {
        draw_labeled_point(image, 2, point, segment.is_horizontal);
      }
    }

    for (auto &segment : segments)
    {
      for (const auto &point : segment.under_other_object)
      {
        if (is_intersection(image, point, segment.is_horizontal))
          segment.points.push_back(point);
      }

      segment.under_other_object.clear();
    }

    return segments;
  }

  /**
   * Post process segments linking them and removing duplications
   * @param pair Pair (horizontal segments,vertical segments)
   * @param img_width Width of the image where segments were extract
   * @param img_height Height of the image where segments were extract
   */
  std::vector<Segment> post_process(std::pair<std::vector<Segment>, std::vector<Segment>>& pair, size_t img_width, size_t img_height, uint32_t min_len,
                    Parameters params)
  {
    segment_linking(pair, params);
    remove_duplicates(pair, img_width, img_height, params);

    auto pre_res = filter_length(pair, min_len);
    auto res = retreive_good_under_other(pre_res, img_width, img_height);

    return res;
  }
} // namespace mln::contrib::segdet