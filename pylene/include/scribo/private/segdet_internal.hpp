#pragma once

#include <scribo/private/segment.hpp>
#include <scribo/segdet.hpp>

namespace scribo::internal
{
  std::vector<Segment> detect_line_seg(const mln::image2d<uint8_t>& image, int min_len, const SegDetParams& params);
  void detect_line_label(mln::image2d<std::uint8_t> image, int min_len, mln::image2d<std::uint16_t> out, const SegDetParams& params);
}