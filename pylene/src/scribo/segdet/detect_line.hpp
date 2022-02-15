#pragma once

#include <mln/core/image/ndimage_fwd.hpp>

#include <cstdint>
#include <vector>

namespace scribo
{
  struct SegDetParams;
}

namespace scribo::internal
{
  using namespace mln;
  struct Segment;


  image2d<uint8_t> preprocess(const image2d<uint8_t>& img, const SegDetParams& params);

  /**
   * Compute the two traversals to detect horizontal and vertical segments
   * @param image image to extract segment from
   * @param min_len
   * @return Pair (horizontal segments,vertical segments)
   */
  std::pair<std::vector<Segment>, std::vector<Segment>> process(const image2d<uint8_t>& image, int min_len_embryo,
                                                                const SegDetParams& params);

  /**
   * Post process segments linking them and removing duplications
   * @param pair Pair (horizontal segments,vertical segments)
   * @param img_width Width of the image where segments were extract
   * @param img_height Height of the image where segments were extract
   */
  std::vector<Segment> post_process(std::vector<Segment>& hsegments, std::vector<Segment>& vsegments, int img_width,
                                    int img_height, int min_len, const SegDetParams& params);
} // namespace mln::contrib::segdet