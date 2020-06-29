#include <mln/core/algorithm/transform.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/neighborhood/c8.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/core/se/mask2d.hpp>
#include <mln/core/colors.hpp>
#include <mln/data/stretch.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <mln/labeling/chamfer_distance_transform.hpp>
#include <mln/morpho/dilation.hpp>
#include <mln/morpho/watershed.hpp>

#include <cstdint>
#include "lut.hpp"


int main(int argc, char** argv)
{
  using namespace mln::view::ops;

  if (argc < 4)
  {
    std::cout << "Usage:" << argv[0] << " input distance output\n";
    std::abort();
  }

  mln::image2d<uint8_t> input;
  mln::io::imread(argv[1], input);

  // BEGIN
  // (1) Compute the distance transform
  mln::se::wmask2d weights = {{3, 2, 3}, //
                                            {2, 0, 2},
                                            {3, 2, 3}};

  auto d = mln::labeling::chamfer_distance_transform<uint8_t>(input, weights);

  // Remove non-meaninfull extrema
  d = mln::morpho::dilation(d, mln::se::disc(5));

  // (2) Inverse the distance
  auto dinv = mln::transform(d, [](uint8_t x) -> uint8_t { return UINT8_MAX - x; });

  // (3) Run the watershed segmentation
  int  nlabel;
  auto ws = mln::morpho::watershed<int16_t>(dinv, mln::c8, nlabel);

  // (4) Labelize input
  auto output = mln::view::ifelse(ws == 0, 1, mln::view::ifelse(input, ws, 0));

  // END
  auto d_stretched = mln::data::stretch<float>(d);

  mln::io::imsave(mln::transform(d_stretched, heat_lut), argv[2]);
  mln::io::imsave(mln::transform(output, regions_lut), argv[3]);
}
