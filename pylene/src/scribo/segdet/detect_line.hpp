#pragma once

#include <cstdint>
#include <tuple>
#include <vector>

#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/ndimage_fwd.hpp>

#include "descriptor.hpp"
#include "segment.hpp"

namespace scribo::internal
{
  using namespace mln;

  /**
   * Preprocess the image if needed
   * @param img Image to preprocess
   * @param descriptor Parameters of the line detection including preprocessing parameters
   */
  image2d<uint8_t> preprocess(const image2d<uint8_t>& img, const Descriptor& descriptor);

  /**
   * Compute the two traversals to detect horizontal and vertical segments
   * @param image image to extract segment from
   * @param descriptor Parameters of the line detection
   * @return Pair (horizontal segments,vertical segments)
   */
  std::pair<std::vector<Segment>, std::vector<Segment>> process(const image2d<uint8_t>& img,
                                                                const Descriptor&       descriptor);

  /**
   * Post process segments linking them and removing duplications
   * @param pair Pair (horizontal segments,vertical segments)
   * @param img_width Width of the image where segments were extract
   * @param img_height Height of the image where segments were extract
   * @param descriptor Parameters of the line detection
   * @return Pair (horizontal segments,vertical segments) after post processing
   * The first vector contains the true segments and the second the object that are not considered has segment by the
   * user but was by the algorithm. Thus their pixel in case of pixel output should not be erased.
   */
  std::tuple<std::vector<Segment>, std::vector<Segment>>
  post_process(std::pair<std::vector<Segment>, std::vector<Segment>> segments_pair, int img_width, int img_height,
               const Descriptor& descriptor);

  /**
   * @brief Perform line detection using functions above
   *
   * @param img The image to process
   * @param min_len The minimum length of segment/lines identified
   * @param params Parameters of the line detection
   * @return Pair (horizontal segments,vertical segments)
   */
  std::tuple<std::vector<Segment>, std::vector<Segment>> detect_line(const image2d<std::uint8_t>& img, int min_len,
                                                                     const SegDetParams& params);
} // namespace scribo::internal