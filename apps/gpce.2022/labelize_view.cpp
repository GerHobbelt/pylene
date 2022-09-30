#include "labelize.hpp"

#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/for_each.hpp>
#include <mln/core/algorithm/transform.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/filter.hpp>
#include <mln/core/image/view/rgb.hpp>
#include <mln/core/image/view/transform.hpp>
#include <mln/core/image/view/zip.hpp>
#include <mln/core/neighborhood/c8.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <mln/labeling/blobs.hpp>
#include <mln/morpho/dilation.hpp>
#include <mln/morpho/watershed.hpp>

#include <cmath>
#include <limits>
#include <string>
#include <string_view>
#include <utility>
#include <vector>


std::tuple<mln::image2d<uint8_t>, int> labelize_view(const mln::image2d<mln::rgb8>& input) {
  auto grayscale = [](auto v) -> uint8_t { return 0.2126 * v[0] + 0.7152 * v[1] + 0.0722 * v[2]; };

    auto ima_grayscale = mln::view::transform(input, grayscale);

    uint8_t threshold    = 125;
    auto    thresholding = [threshold](auto v) -> uint8_t { return v > threshold ? 255 : 0; };

    auto input_thresholded = mln::view::transform(ima_grayscale, thresholding);

    // perform connected components labeling
    int nlabel;
    auto ima_ret = mln::labeling::blobs<uint8_t>(input_thresholded, mln::c8, nlabel);
    return std::make_tuple(ima_ret, nlabel);
}
