#include <mln/core/colors.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/core/image/view/operators.hpp>
#include <mln/core/image/view/transform.hpp>
#include <mln/core/neighborhood/c8.hpp>
#include <mln/core/se/disc.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <mln/morpho/gradient.hpp>
#include <mln/morpho/watershed.hpp>

#include <iostream>

#include "lut.hpp"

void create_lena_seeds(mln::image2d<std::uint16_t>& output)
{
  auto params       = mln::image_build_params();
  params.init_value = std::make_any<std::uint16_t>(0);
  output.resize({512, 512}, params);
  mln::point2d seeds[] = {{123, 39},  {253, 133}, {488, 342}, {449, 265}, {289, 463}, {299, 304},
                          {106, 470}, {45, 230},  {12, 299},  {463, 88},  {387, 47},  {467, 18}};
  for (auto&& p : seeds)
    output(p) = 1;
}

int main(int argc, char* argv[])
{
  if (argc < 4)
  {
    std::cerr << "Usage: " << argv[0] << " input output_gradient output_watershed seeds[uint16]\n";
    return 1;
  }

  mln::image2d<std::uint8_t>  input;
  mln::image2d<std::uint16_t> seeds;
  mln::io::imread(argv[1], input);
  if (argc == 4)
  {
    std::cerr << "[WARNING]: Using seeds for lena\n";
    create_lena_seeds(seeds);
  }
  else
    mln::io::imread(argv[4], seeds);
  if (seeds.domain() != input.domain())
    throw std::invalid_argument("Input image and seeds should have the same domain");

  int  nlbl;
  auto grad = mln::morpho::gradient(input, mln::se::disc(3));
  auto out  = mln::morpho::watershed_from_markers<std::int32_t>(grad, mln::c8, seeds, nlbl);
  auto res  = mln::view::transform(out, regions_lut);

  {
    auto to_save = mln::view::ifelse(mln::morpho::dilation(seeds, mln::se::disc(5)), mln::rgb8{255, 0, 0}, grad);
    mln::io::imsave(to_save, argv[2]);
  }

  mln::io::imsave(res, argv[3]);
  std::cout << "Label: " << nlbl << "\n";
  return 0;
}