#pragma once

#include "segment.hpp"
#include <mln/core/image/ndbuffer_image.hpp>

namespace mln
{
  namespace contrib
  {
    namespace segdet
    {
      int detect_line(ndbuffer_image image, uint min_len, uint discontinuity);

      std::pair<std::vector<Segment>, std::vector<Segment>> process(const ndbuffer_image& image, uint min_len, uint discontinuity);

    }
  }
}