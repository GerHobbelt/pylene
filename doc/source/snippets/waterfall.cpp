#include <mln/core/image/view/cast.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/neighborhood/c8.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <mln/labeling/blobs.hpp>
#include <mln/morpho/gradient.hpp>
#include <mln/morpho/waterfall.hpp>

#include <iostream>
#include <string>

#include "lut.hpp"

int main(int argc, char* argv[])
{
  using namespace mln::view::ops;
  if (argc < 5)
  {
    std::cerr << "Usage: " << argv[0] << " threshold input output_watershed output_segmentation\n";
    return 1;
  }
  int         threshold = std::atoi(argv[1]);
  std::string input_filename(argv[2]);
  std::string output_watershed(argv[3]);
  std::string output_segmentation_filename(argv[4]);

  // Reading the input image
  mln::image2d<std::uint8_t> input;
  mln::io::imread(input_filename, input);

  // Computing the gradient
  auto grad = mln::morpho::gradient(input, mln::se::disc(3));

  // Computing the waterfall
  auto [t, nodemap] = mln::morpho::waterfall(grad, mln::c8);

  // Save the watershed
  mln::io::imsave(mln::view::transform(nodemap, [](auto x) { return regions_lut(x); }), output_watershed);

  auto cut = t.horizontal_cut(threshold, nodemap);
  mln::io::imsave(mln::view::transform(cut, [](auto x) { return regions_lut(x); }), output_segmentation_filename);

  return 0;
}