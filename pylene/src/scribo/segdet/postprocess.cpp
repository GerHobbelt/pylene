#include "detect_line.hpp"
#include "label_array.hpp"

#include <mln/core/algorithm/fill.hpp>
#include <mln/core/image/view/transform.hpp>
#include <mln/core/range/foreach.hpp>
#include <scribo/segdet.hpp>

#include <cmath>


namespace scribo::internal
{
  /**
   * Compute the distance between 2 spans
   * @param p1
   * @param p2
   * @return
   */
  float distance_spans(const Span& p1, const Span& p2)
  {
    int xvar = p1.x - p2.x;
    int yvar = p1.y - p2.y;

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
  float distance_linking(Segment& a, const Segment& b, const SegDetParams& params)
  {
    if (std::abs(a.last_part_slope - b.first_part_slope) > params.merge_slope_variation)
      return params.merge_distance_max;

    return distance_spans(a.last_span, b.first_span);
  }

  /**
   * Merge the second segment to the first
   * @param a First segment
   * @param b Second segment
   */
  void merge_segments(Segment& a, const Segment& b)
  {
    if (a.is_horizontal)
      a.length = b.spans[b.spans.size() - 1].x - a.spans[0].x + 1;
    else
      a.length = b.spans[b.spans.size() - 1].y - a.spans[0].y + 1;

    for (auto& p : b.spans)
      a.spans.push_back(p);
    for (auto& p : b.under_other_object)
      a.under_other_object.push_back(p);

    a.nb_pixels += b.nb_pixels;

    a.last_part_slope = b.last_part_slope;
    a.last_span       = b.last_span;
  }

  /**
   * Call the function linking segments if needed
   * @param segments All segments
   */
  void segment_link(std::vector<Segment>& segments, const SegDetParams& params)
  {
    int i = 0;
    while (i < (int)segments.size())
    {
      int j = i + 1;

      int   best_index    = i;
      float best_distance = params.merge_distance_max;
      while (j < (int)segments.size())
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
      else
        i++;
    }
  }

  /**
   * Call segment link for horizontal and vertical segments
   * @param pair First is horizontal segment and second vertical segments
   */
  void segment_linking(std::pair<std::vector<Segment>, std::vector<Segment>>& pair, const SegDetParams& params)
  {
    segment_link(pair.first, params);
    segment_link(pair.second, params);
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
  void intersect(image2d<std::uint16_t> img, image2d<std::uint16_t> img2, image2d<std::uint16_t> out)
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
  void remove_dup(std::vector<Segment>& segments_to_compare, std::vector<Segment>& segments_removable, int width,
                  int height, const SegDetParams& params)
  {
    image2d<std::uint16_t> first_output = image2d<std::uint16_t>(width, height);
    mln::fill(first_output, 0);
    labeled_arr(first_output, segments_to_compare, segments_removable, LABELING_TYPE_HORIZONTAL);

    image2d<std::uint16_t> second_output = image2d<std::uint16_t>(width, height);
    mln::fill(second_output, 0);
    labeled_arr(second_output, segments_to_compare, segments_removable, LABELING_TYPE_VERTICAL);

    auto second_output_bin =
        mln::view::transform(second_output, [](std::uint16_t p) -> std::uint8_t { return (p != 0) ? 1 : 0; });

    binarize_img(first_output);

    image2d<std::uint16_t> intersection = image2d<std::uint16_t>(width, height);
    mln::fill(intersection, 0);

    intersect(first_output, second_output, intersection);

    std::vector<uint16_t> segments(segments_removable.size(), 0);

    mln_foreach (auto v, intersection.values())
    {
      if (v >= 3)
        segments[v - 3]++;
    }

    int k = 0;
    for (int i = 0; i < (int)segments.size(); i++)
    {
      float segments_ratio = 0;
      if (segments_removable[i - k].nb_pixels != 0)
        segments_ratio = segments[i] / static_cast<float>(segments_removable[i - k].nb_pixels);
      if (segments_removable[i - k].nb_pixels == 0 || segments_ratio > params.threshold_intersection)
      {
        segments_removable.erase(segments_removable.begin() + i - k);
        k++;
      }
    }
  }

  /**
   * Call remove duplicates for horizontal and vertical segments
   * @param pair Pair (horizontal segments,vertical segments)
   * @param img_width Width of the image where segments were extract
   * @param img_height Height of the image where segments were extract
   */
  void remove_duplicates(std::pair<std::vector<Segment>, std::vector<Segment>>& pair, int img_width, int img_height,
                         const SegDetParams& params)
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
  std::vector<Segment> filter_length(std::pair<std::vector<Segment>, std::vector<Segment>>& p, int min_len)
  {
    std::vector<Segment> res{};

    for (auto& seg : p.first)
    {
      if (seg.length >= min_len)
        res.push_back(seg);
    }
    for (auto& seg : p.second)
    {
      if (seg.length >= min_len)
        res.push_back(seg);
    }

    return res;
  }

  bool is_usefull_under_object(image2d<std::uint16_t> img, Span& span, bool is_horizontal)
  {
    auto thickness = span.thickness / 2;
    int  is_odd    = span.thickness % 2;

    if (is_horizontal)
    {
      int y_init = span.y - thickness;
      int y_end  = span.y + thickness + is_odd;
      if (thickness != 0)
        y_end--;

      while (y_init < 0)
        y_init++;
      while (y_end >= img.size(1))
        y_end--;

      while (y_init <= y_end && img({static_cast<int>(span.x), y_init}) == 2)
        y_init++;
      while (y_init <= y_end && img({static_cast<int>(span.x), y_end}) == 2)
        y_end--;

      span.thickness = y_end - y_init + 1;
      span.y         = (y_init + y_end) / 2;

      if (span.thickness < 1)
      {
        // std::cout << "toto" << span.x << " " << span.y << " " << span.thickness << "\n";
        return false;
      }
    }
    else
    {
      int x_init = span.x - thickness;
      int x_end  = span.x + thickness + is_odd;
      if (thickness != 0)
        x_end--;

      while (x_init < 0)
        x_init++;
      while (x_end >= img.size(0))
        x_end--;

      int y = static_cast<int>(span.y);
      while (x_init <= x_end && img({x_init, y}) == 2)
        x_init++;
      while (x_init <= x_end && img({x_end, y}) == 2)
        x_end--;

      span.thickness = x_end - x_init + 1;
      span.x         = (x_init + x_end) / 2;

      if (span.thickness < 1)
        return false;
    }

    return true;
  }

  std::vector<Segment> retrieve_good_under_other(std::vector<Segment>& segments, int width, int height)
  {
    image2d<std::uint16_t> image = image2d<std::uint16_t>(width, height);
    mln::fill(image, 0);

    for (const auto& segment : segments)
    {
      for (const auto& span : segment.under_other_object)
        draw_labeled_span(image, 2, span, segment.is_horizontal);

      for (const auto& span : segment.spans)
        draw_labeled_span(image, 2, span, segment.is_horizontal);
    }

    for (auto& segment : segments)
    {
      for (auto& span : segment.under_other_object)
      {
        if (is_usefull_under_object(image, span, segment.is_horizontal))
        {
          segment.spans.push_back(span);
          segment.nb_pixels += span.thickness;
        }
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
  std::vector<Segment> post_process(std::vector<Segment>& hsegments, std::vector<Segment>& vsegments, int img_width,
                                    int img_height, int min_len, const SegDetParams& params)
  {
    auto pair = std::make_pair(hsegments, vsegments);
    segment_linking(pair, params);
    remove_duplicates(pair, img_width, img_height, params);

    auto segments = filter_length(pair, min_len);
    //    auto res      = retrieve_good_under_other(segments, img_width, img_height);

    return segments;
  }
} // namespace scribo::internal