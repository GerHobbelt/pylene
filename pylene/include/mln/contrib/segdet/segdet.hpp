#pragma once

#include "segment.hpp"
#include <mln/core/image/ndbuffer_image.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/rgb.hpp>
#include <mln/io/imread.hpp>
#include <mln/core/colors.hpp>

#include <mln/morpho/erosion.hpp>
#include <utility>


namespace mln::contrib::segdet
    {

      /**
       * Detects lines in the given image
       * @param image A image2d<uint8> representing the image to process
       * @param min_len The minimum length of segments to detect
       * @param discontinuity The maximum accepted discontinuity for segments
       * @return A vector of detected segments
       */
      std::vector<Segment> detect_line(image2d<uint8_t> image, uint min_len, uint discontinuity);

      /**
       * Process the image
       * @param image The image to process
       * @param min_len The minimum length of segments to detect
       * @param discontinuity The maximum accepted discontinuity
       * @return
       */
      std::pair<std::vector<Segment>, std::vector<Segment>> process(const image2d<uint8_t>& image, uint min_len,
                                                                    uint discontinuity);


      mln::image2d<uint8_t> preprocess_img(mln::image2d<mln::rgb8> img);


    } // namespace mln