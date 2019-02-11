#include "detect_line.hpp"
#include "segment_to_X.hpp"

#include <scribo/segdet.hpp>

namespace scribo
{
  using namespace internal;

  std::vector<VSegment> detect_line_vector(const mln::image2d<std::uint8_t>& image, int min_len,
                                           const SegDetParams& params)
  {
    auto segs = detect_line(std::move(image), min_len, params);
    auto ret  = segment_to_vsegment(segs);
    return ret;
  }

  std::tuple<mln::image2d<uint16_t>, std::vector<LSuperposition>>
  detect_line_label(mln::image2d<std::uint8_t> img_in, int min_len, const SegDetParams& params)
  {
    auto segs = detect_line(std::move(img_in), min_len, params);
    auto ret  = segment_to_label(segs, img_in.size(0), img_in.size(1));
    return ret;
  }

  std::tuple<mln::image2d<std::uint16_t>, std::vector<LSuperposition>, std::vector<VSegment>>
  detect_line_full(mln::image2d<std::uint8_t> image, int min_len, const SegDetParams& params)
  {
    auto segs = detect_line(std::move(image), min_len, params);

    auto vsegs  = segment_to_vsegment(segs);
    auto labels = segment_to_label(segs, image.size(0), image.size(1));

    auto ret = std::make_tuple(std::get<0>(labels), std::get<1>(labels), vsegs);
    return ret;
  }
} // namespace scribo