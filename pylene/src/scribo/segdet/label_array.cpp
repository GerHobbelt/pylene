#include "label_array.hpp"

namespace scribo::internal
{
  /**
   * Label a pixel in img
   * @param img Image to labelize
   * @param label Color
   * @param x X position
   * @param y Y position
   */
  void draw_labeled_pixel(image2d<uint16_t>& img, std::uint16_t label, int x, int y)
  {
    img({x, y}) = img({x, y}) != 0 ? 1 : label;
  }

  /**
   * Draw a pint in img
   * @param img Image to labelize
   * @param label Color
   * @param span Span to draw
   * @param is_horizontal
   */
  void draw_labeled_span(image2d<uint16_t>& img, std::uint16_t label, Span span, bool is_horizontal)
  {
    int thickness = span.thickness / 2;
    int is_odd    = span.thickness % 2;

    if (is_horizontal)
    {
      for (int i = -thickness; i < thickness + is_odd; i++)
      {
        int y = span.y + i;
        if (0 <= y && y < img.size(1))
          draw_labeled_pixel(img, label, span.x, y);
      }
    }
    else
    {
      for (int i = -thickness; i < thickness + is_odd; i++)
      {
        int x = span.x + i;
        if (0 <= x && x < img.size(0))
          draw_labeled_pixel(img, label, x, span.y);
      }
    }
  }


  void labeled_arr(image2d<uint16_t> out, const std::vector<Segment>& segments, int first_label)
  {
    for (int i = 0; i < (int)segments.size(); i++)
    {
      for (auto& span : segments[i].spans)
        draw_labeled_span(out, i + 3 + first_label, span, segments[i].is_horizontal);
    }
  }
} // namespace mln::contrib::segdet