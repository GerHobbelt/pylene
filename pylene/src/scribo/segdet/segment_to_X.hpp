#pragma once

#include <tuple>
#include <vector>

#include <mln/core/image/ndbuffer_image.hpp>
#include <mln/core/image/ndimage.hpp>
#include <scribo/segdet.hpp>

#include "segment.hpp"

namespace scribo::internal
{
  using namespace mln;

  /**
   * Draw segments in img out
   * @param out Image to draw in
   * @param segments Segments identified in image
   *
   * label 0 : No segment on the current position
   * label 1 : Reserved for superposition
   * label X : Segment X
   */
  std::tuple<image2d<std::uint16_t>, std::vector<LSuperposition>>
  segment_to_label(const std::vector<Segment>& segments,const std::vector<Segment>& nsegments, int width, int height, bool handle_under_other = true);

  std::vector<VSegment> segment_to_vsegment(const std::vector<Segment>& segs);
} // namespace scribo::internal