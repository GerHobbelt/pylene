#include "detect_line.hpp"

namespace scribo::internal
{
  using namespace mln;

  std::tuple<std::vector<Segment>, std::vector<Segment>> detect_line(const image2d<std::uint8_t>& image, int min_len, const SegDetParams& params)
  {
    // Parameter setting
    const Descriptor& descriptor = Descriptor(params, min_len);

    // Preprocessing
    image2d<std::uint8_t> preprocessed_img = preprocess(std::move(image), descriptor);

    // Processing
    auto p = process(std::move(preprocessed_img), descriptor);

    // Post Processing
    auto result = post_process(p, image.size(0), image.size(1), descriptor);

    return result;
  }
} // namespace scribo::internal