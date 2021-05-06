#pragma once

#include "segment.hpp"
#include <mln/core/colors.hpp>
#include <mln/core/image/ndbuffer_image.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/rgb.hpp>
#include <mln/io/imread.hpp>

#include <mln/morpho/erosion.hpp>
#include <utility>


namespace mln::contrib::segdet
{

  /**
   * Detects lines in the given image
   * @param image A ndbuffer representing the image to process (can be rgb or uint8)
   * @param min_len The minimum length of segments to detect
   * @param discontinuity The maximum accepted discontinuity for segments
   * @return A vector of detected segments
   */
  std::vector<Segment> detect_line(mln::ndbuffer_image image, uint min_len, uint discontinuity);

} // namespace mln::contrib::segdet