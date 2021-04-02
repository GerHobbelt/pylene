

#include "mln/contrib/segdet/segdet.hpp"
#include <mln/contrib/segdet/filter.hpp>

namespace mln::contrib::segdet
{
  int detect_line(mln::ndbuffer_image image, uint min_len, uint discontinuity)
  {
    // TODO faire le top hat


    return 0;
  }
  std::vector<Segment> dothething(ndbuffer_image image, bool is_horizontal, uint min_len, uint discontinuity);
  std::pair<std::vector<Segment>, std::vector<Segment>> process(const mln::ndbuffer_image& image, uint min_len,
                                                                uint discontinuity)
  {
    // TODO Multi threading, splitter l'image
    std::vector<Segment> horizontal_segments = dothething(image, true, min_len, discontinuity);
    std::vector<Segment> vertical_segments   = dothething(image, false, min_len, discontinuity);

    return std::make_pair(horizontal_segments, vertical_segments);
  }

  std::vector<Segment> dothething(const ndbuffer_image& image, bool is_horizontal, uint min_len, uint discontinuity)
  {
    uint32_t xmult     = 1;
    uint32_t ymult     = 0;
    double   slope_max = SEGDET_SLOPE_MAX_VERTICAL;
    if (is_horizontal)
    {
      xmult     = 0;
      ymult     = 1;
      slope_max = SEGDET_SLOPE_MAX_HORIZONTAL;
    }

    uint32_t n_max = image.size() * ymult + image.size(1) * xmult;
    uint32_t t_max = image.size() * xmult + image.size(1) * ymult;

    auto filters  = std::vector<Filter>();
    auto segments = std::vector<Segment>();

    uint32_t two_matches = 0;

    for (uint32_t t = 0; t < t_max; t++)
    {
      for (auto& filter : filters)
      {
        predict(filter, t)
      }
    }

    return std::vector<Segment>();
  }
} // namespace mln::contrib::segdet