#pragma once

#include <scribo/segdet.hpp>

namespace scribo::internal
{
  // Used for tests
  struct segdet_output
  {
    std::vector<int>   seg_ids;
    std::vector<float> mid_pos_x;
    std::vector<float> mid_pos_y;
    std::vector<int>   thickness;
    std::vector<bool>  angle;
  };

  segdet_output detect_line_span(const mln::image2d<std::uint8_t>& image, int min_len, const SegDetParams& params);
  segdet_output detect_line_span(const mln::image2d<std::uint8_t>& image, int min_len);

} // namespace scribo::internal