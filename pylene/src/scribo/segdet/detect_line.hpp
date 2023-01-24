#pragma once

#include <cstdint>
#include <vector>
#include <tuple>

#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/ndimage_fwd.hpp>

#include "descriptor.hpp"
#include "segment.hpp"

namespace scribo::internal
{
  using namespace mln;

  image2d<uint8_t> preprocess(const image2d<uint8_t>& img, const Descriptor& descriptor);

  /**
   * Compute the two traversals to detect horizontal and vertical segments
   * @param image image to extract segment from
   * @param min_len
   * @param discontinuity
   * @return Pair (horizontal segments,vertical segments)
   */
  std::pair<std::vector<Segment>, std::vector<Segment>> process(const image2d<uint8_t>& img,
                                                                const Descriptor&       descriptor);

  /**
   * Post process segments linking them and removing duplications
   * @param pair Pair (horizontal segments,vertical segments)
   * @param img_width Width of the image where segments were extract
   * @param img_height Height of the image where segments were extract
   */
  std::tuple<std::vector<Segment>, std::vector<Segment>> post_process(std::vector<Segment>& hsegments, std::vector<Segment>& vsegments, int img_width,
                                    int img_height, const Descriptor& descriptor);

  /**
   * @brief Perform line detection using functions above
   * 
   * @param img The image to process
   * @param min_len The minimum length of segment/lines identified
   * @param params Parameters of the line detection
   * @return std::vector<Segment> 
   */
  std::tuple<std::vector<Segment>, std::vector<Segment>> detect_line(const image2d<std::uint8_t>& img, int min_len, const SegDetParams& params);
} // namespace scribo::internal