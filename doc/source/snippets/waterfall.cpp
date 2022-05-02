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
  if (argc < 6)
  {
    std::cerr << "Usage: " << argv[0] << " threshold input output_watershed output_saliency output_segmentation\n";
    return 1;
  }
  int         threshold = std::atoi(argv[1]);
  std::string input_filename(argv[2]);
  std::string output_watershed(argv[3]);
  std::string output_saliency_filename(argv[4]);
  std::string output_segmentation_filename(argv[5]);

  // Reading the input image
  mln::image2d<std::uint8_t> input;
  mln::io::imread(input_filename, input);

  // Computing the gradient
  auto grad = mln::morpho::gradient(input, mln::se::disc(3));

  // Computing the waterfall
  auto [t, nodemap] = mln::morpho::waterfall(grad, mln::c8);

  // Save the watershed
  {
    const int waterline = t.parent.size() - 1;
    mln::io::imsave(mln::view::transform(mln::view::ifelse(nodemap == waterline, 0, nodemap),
                                         [](auto x) { return regions_lut(x); }),
                    output_watershed);
  }

  // Compute the saliency map
  auto saliency = mln::morpho::waterfall_saliency(t, nodemap, mln::c8);

  // Save the saliency
  mln::io::imsave(mln::view::transform(saliency,
                                       [&t](int x) -> std::uint8_t {
                                         return static_cast<float>(x) / static_cast<float>(t.values[0]) * 255;
                                       }),
                  output_saliency_filename.c_str());

  // Threshold the saliency
  auto thresholded_saliency = saliency >= threshold;
  mln::io::imsave(thresholded_saliency, "debug.pgm");
  int  nlbl;
  auto labeled       = mln::labeling::blobs<std::uint16_t>(1 - thresholded_saliency, mln::c8, nlbl);
  auto labeled_color = mln::view::transform(labeled, [](auto x) { return regions_lut(x); });
  mln::io::imsave(labeled_color, output_segmentation_filename);

  return 0;
}