#include "detect_line.hpp"
#include "segment_to_X.hpp"

#include <scribo/segdet.hpp>

namespace scribo
{
  using namespace internal;

  std::vector<VSegment> detect_line_vector(const mln::image2d<std::uint8_t>& image, int min_len,
                                           const SegDetParams& params)
  {
    auto [segs, _] = detect_line(image, min_len, params);
    auto ret       = segment_to_vsegment(segs);
    return ret;
  }

  std::tuple<mln::image2d<uint16_t>, std::vector<LSuperposition>>
  detect_line_label(const mln::image2d<std::uint8_t>& img_in, int min_len, const SegDetParams& params)
  {
    auto [segs, nsegs] = detect_line(img_in, min_len, params);
    auto ret           = segment_to_label(segs, nsegs, img_in.size(0), img_in.size(1));

    return ret;
  }

  std::tuple<mln::image2d<std::uint16_t>, std::vector<LSuperposition>, std::vector<VSegment>>
  detect_line_full(const mln::image2d<std::uint8_t>& image, int min_len, const SegDetParams& params)
  {
    auto [segs, nsegs] = detect_line(image, min_len, params);

    auto vsegs  = segment_to_vsegment(segs);
    auto labels = segment_to_label(segs, nsegs, image.size(0), image.size(1));

    auto ret = std::make_tuple(std::get<0>(labels), std::get<1>(labels), vsegs);
    return ret;
  }
} // namespace scribo