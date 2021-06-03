#include <mln/contrib/segdet/segdet.hpp>

#include <algorithm>
#include <mln/contrib/segdet/detect_line.hpp>
#include <mln/core/image/view/transform.hpp>
#include <utility>

namespace mln::contrib::segdet
{
  /**
   * Only keep segments that are above a given length and combine pair
   * @param p Pair of vectors of segments
   * @param min_len The length
   * @return
   */
  std::vector<Segment> filter_length(std::pair<std::vector<Segment>, std::vector<Segment>>& p, uint min_len)
  {
    std::vector<Segment> res{};

    for (auto& seg : p.first)
    {
      if (seg.length > min_len)
        res.push_back(seg);
    }
    for (auto& seg : p.second)
    {
      if (seg.length > min_len)
        res.push_back(seg);
    }

    return res;
  }

  // Public functions

  std::vector<Segment> detect_line(mln::ndbuffer_image image, uint min_len, uint discontinuity,
                                   const Parameters& params)
  {
    // Preprocessing
    mln::ndbuffer_image preprocessed_img = preprocess(std::move(image));

    uint min_len_embryo = min_len / 4 + 1; // 5U < min_len ? 5U : min_len;
    // Processing
    auto p = process(preprocessed_img.__cast<uint8_t, 2>(), min_len_embryo, discontinuity, params);

    // Post Processing
    post_process(p, preprocessed_img.size(0), preprocessed_img.size(1), params);

    auto res = filter_length(p, min_len);

    return res;
  }

  std::vector<Segment> detect_line(mln::ndbuffer_image image, uint min_len, uint discontinuity)
  {
    return detect_line(std::move(image), min_len, discontinuity, Parameters());
  }
} // namespace mln::contrib::segdet