#include "segment_to_X.hpp"

#include <mln/core/algorithm/fill.hpp>
#include <mln/core/range/foreach.hpp>


namespace scribo::internal
{
  /**
   * Label a pixel in img
   * @param img Image to labelize
   * @param label Color
   * @param x X position
   * @param y Y position
   */
  void draw_labeled_pixel(image2d<uint16_t>& img, std::uint16_t label, int x, int y,
                          std::vector<LSuperposition>& superpositions, uint16_t max_label)

  {
    auto pos = {x, y};
    if (img(pos) == 0 || label == 0)
    {
      img(pos) = label + max_label;
    }
    else
    {
      uint16_t last_label = img(pos);
      if (last_label > max_label)
        last_label -= max_label;

      if (last_label != 1)
        superpositions.push_back({.label = last_label, .x = x, .y = y});
      superpositions.push_back({.label = label, .x = x, .y = y});

      img(pos) = 1;
    }
  }

  /**
   * Draw a pint in img
   * @param img Image to labelize
   * @param label Color
   * @param span Span to draw
   * @param is_horizontal
   */
  void draw_labeled_span(image2d<uint16_t>& img, std::uint16_t label, Span span, bool is_horizontal,
                         std::vector<LSuperposition>& superpositions, uint16_t max_label)
  {
    float thickness_d2 = static_cast<float>(span.thickness) / 2.0f;

    if (is_horizontal)
    {
      int i_min = std::ceil(span.y - thickness_d2);
      int i_max = std::floor(span.y + thickness_d2);

      int height = img.size(1);

      for (int i = i_min; i <= i_max; i++)
      {
        if (0 <= i && i < height)
          draw_labeled_pixel(img, label, span.x, i, superpositions, max_label);
      }
    }
    else
    {
      int i_min = std::ceil(span.x - thickness_d2);
      int i_max = std::floor(span.x + thickness_d2);
      int width = img.size(0);

      for (int i = i_min; i <= i_max; i++)
      {
        if (0 <= i && i < width)
          draw_labeled_pixel(img, label, i, span.y, superpositions, max_label);
      }
    }
  }

  std::tuple<image2d<std::uint16_t>, std::vector<LSuperposition>> segment_to_label(const std::vector<Segment>& segments,
                                                                                   const std::vector<Segment>& nsegment,
                                                                                   int width, int height,
                                                                                   bool handle_under_other)
  {
    auto img_out = mln::image2d<uint16_t>(width, height);
    mln::fill(img_out, 0);

    std::vector<LSuperposition> superpositions;

    int max_label = static_cast<int>(segments.size()) + first_label;

    for (int i = 0; i < (int)segments.size(); i++)
    {
      for (auto& span : segments[i].spans)
        draw_labeled_span(img_out, i + first_label, span, segments[i].is_horizontal, superpositions, 0);

      if (handle_under_other)
        for (auto& span : segments[i].under_other_object)
          draw_labeled_span(img_out, i + first_label, span, segments[i].is_horizontal, superpositions, max_label);
    }

    // Remove object that are not segments
    for (int i = 0; i < (int)nsegment.size(); i++)
    {
      for (auto& span : nsegment[i].spans)
        draw_labeled_span(img_out, 0, span, nsegment[i].is_horizontal, superpositions, 0);

      for (auto& span : nsegment[i].under_other_object)
        draw_labeled_span(img_out, 0, span, nsegment[i].is_horizontal, superpositions, 0);
    }

    if (handle_under_other)
    {
      mln_foreach (auto p, img_out.domain())
      {
        if (img_out(p) > max_label)
          img_out(p) = 0;
      }
    }

    return std::make_tuple(img_out, superpositions);
  }

  std::vector<VSegment> segment_to_vsegment(const std::vector<Segment>& segs)
  {
    std::vector<VSegment> ret;
    uint16_t              label_counter = first_label;
    for (const auto& seg : segs)
    {
      VSegment vseg = {.label = label_counter++,
                       .x0    = static_cast<int>(seg.first_span.x),
                       .y0    = static_cast<int>(seg.first_span.y),
                       .x1    = static_cast<int>(seg.last_span.x),
                       .y1    = static_cast<int>(seg.last_span.y)};
      ret.push_back(vseg);
    }

    return ret;
  }
} // namespace scribo::internal