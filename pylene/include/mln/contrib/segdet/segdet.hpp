#pragma once

#include "segment.hpp"
#include <Eigen/Dense>
#include <mln/core/image/ndbuffer_image.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/io/imread.hpp>
#include <utility>


namespace mln
{
  namespace contrib
  {
    namespace segdet
    {

      int detect_line(const image2d<uint8_t>& image, uint min_len, uint discontinuity);

      std::pair<std::vector<Segment>, std::vector<Segment>> process(const image2d<uint8_t>& image, uint min_len,
                                                                    uint discontinuity);
    } // namespace segdet
  }   // namespace contrib
} // namespace mln