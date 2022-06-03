#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/clone.hpp>
#include <mln/core/algorithm/copy.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/algorithm/paste.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/image/image.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/cast.hpp>
#include <mln/core/image/view/channel.hpp>
#include <mln/core/image/view/filter.hpp>
#include <mln/core/image/view/mask.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/image/view/rgb.hpp>
#include <mln/core/image/view/transform.hpp>
#include <mln/core/image/view/zip.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighborhood/c8.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <mln/labeling/blobs.hpp>
#include <mln/morpho/experimental/dilation.hpp>
#include <mln/morpho/experimental/erosion.hpp>
#include <mln/morpho/gaussian2d.hpp>


#include <fmt/core.h>


#include <cmath>
#include <limits>
#include <string>
#include <string_view>
#include <utility>
#include <vector>


int main(int, char** argv)
{
  std::string path = argv[1];

  mln::image2d<uint8_t> input;
  mln::io::imread(path + "images/input.pgm", input);
  mln::io::imsave(input, path + "images/output.pgm");

  uint8_t threshold    = 125;
  auto    thresholding = [threshold](auto v) -> uint8_t { return v > threshold ? 255 : 0; };

  auto input_thresholded = mln::view::transform(input, thresholding);

  mln::io::imsave(input_thresholded, path + "images/input_thresholded.pgm");

  int  nlabel;
  auto out = mln::labeling::blobs<uint8_t>(input_thresholded, mln::c8, nlabel);
  fmt::print("nlabels = {}\n", nlabel);


  mln::io::imsave(out, path + "images/labels.pgm");
}
