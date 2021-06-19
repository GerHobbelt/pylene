#pragma once

#include "segment.hpp"
#include <mln/core/image/ndbuffer_image.hpp>
#include <mln/core/image/ndimage.hpp>

namespace mln::contrib::segdet
{

  mln::ndbuffer_image preprocess(mln::ndbuffer_image img);

  /**
   * Compute the two traversals to detect horizontal and vertical segments
   * @param image image to extract segment from
   * @param min_len
   * @param discontinuity
   * @return Pair (horizontal segments,vertical segments)
   */
  std::pair<std::vector<Segment>, std::vector<Segment>> process(image2d<uint8_t> image, uint min_len_embryo,
                                                                uint discontinuity, Parameters params);

  /**
   * Post process segments linking them and removing duplications
   * @param pair Pair (horizontal segments,vertical segments)
   * @param img_width Width of the image where segments were extract
   * @param img_height Height of the image where segments were extract
   */
  std::vector<Segment> post_process(std::pair<std::vector<Segment>, std::vector<Segment>>& pair, size_t img_width, size_t img_height, uint32_t min_len,
                    Parameters params);

} // namespace mln::contrib::segdet