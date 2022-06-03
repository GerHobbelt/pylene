#pragma once

#include <mln/core/image/ndbuffer_image.hpp>
#include <mln/core/image/ndimage.hpp>
#include <scribo/private/segment.hpp>

namespace scribo::internal
{
  using namespace mln;

  /**
   * Draw segments in img out
   * @param out Image to draw in
   * @param segments
   */
  void labeled_arr(image2d<uint16_t> out, const std::vector<Segment>& segments, int first_label = 0);

  /**
   * Draw a pint in img
   * @param img Image to labelize
   * @param label Color
   * @param span Span to draw
   * @param is_horizontal
   */
  void draw_labeled_span(image2d<uint16_t>& img, uint16_t label, Span span, bool is_horizontal);
} // namespace mln::contrib::segdet