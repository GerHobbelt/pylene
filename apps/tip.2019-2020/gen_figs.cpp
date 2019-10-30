#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/views.hpp>
#include <mln/io/experimental/imread.hpp>
#include <mln/io/experimental/imsave.hpp>

#include <iostream>


int main()
{
  auto lena_dyn = mln::io::experimental::imread("images/lena_color.png");
  auto lena     = *lena_dyn.cast_to<mln::rgb8, 2>();

  mln::io::experimental::imsave(mln::view::red(lena), "images/lena_red_mono.png");
  mln::io::experimental::imsave(mln::view::green(lena), "images/lena_green_mono.png");
  mln::io::experimental::imsave(mln::view::blue(lena), "images/lena_blue_mono.png");

  auto filter_red   = [](auto v) -> mln::rgb8 { return {v[0], 0, 0}; };
  auto filter_green = [](auto v) -> mln::rgb8 { return {0, v[1], 0}; };
  auto filter_blue  = [](auto v) -> mln::rgb8 { return {0, 0, v[2]}; };

  auto lena_red   = mln::view::transform(lena, filter_red);
  auto lena_green = mln::view::transform(lena, filter_green);
  auto lena_blue  = mln::view::transform(lena, filter_blue);

  mln::io::experimental::imsave(lena_red, "images/lena_red.png");
  mln::io::experimental::imsave(lena_green, "images/lena_green.png");
  mln::io::experimental::imsave(lena_blue, "images/lena_blue.png");

  using namespace mln::view::ops;
  auto lena_rg = lena_red + lena_green;
  auto lena_rb = lena_red + lena_blue;
  auto lena_gb = lena_green + lena_blue;
  mln::io::experimental::imsave(lena_rg, "images/lena_rg.png");
  mln::io::experimental::imsave(lena_rb, "images/lena_rb.png");
  mln::io::experimental::imsave(lena_gb, "images/lena_gb.png");

  if (mln::all_of(lena - lena_red == lena_gb))
  {
    std::cout << "rgb - red == green + blue" << std::endl;
  }
  if (mln::all_of(lena - lena_green == lena_rb))
  {
    std::cout << "rgb - green == red + blue" << std::endl;
  }
  if (mln::all_of(lena - lena_blue == lena_rg))
  {
    std::cout << "rgb - blue == red + green" << std::endl;
  }

  mln::io::experimental::imsave(lena_red + lena_green, "images/lena_rg.png");
  mln::io::experimental::imsave(lena_red + lena_blue, "images/lena_rb.png");
  mln::io::experimental::imsave(lena_green + lena_blue, "images/lena_bg.png");

  auto planet_dyn       = mln::io::experimental::imread("images/planet.png");
  auto planet           = *planet_dyn.cast_to<mln::rgb8, 2>();
  auto lena_plus_planet = lena + planet;
  mln::io::experimental::imsave(lena_plus_planet, "images/lena_+_planet.png");

  [[maybe_unused]] auto zipped_ima = mln::view::zip(lena_red, lena_blue);
  [[maybe_unused]] auto out        = imconcretize(lena).set_init_value(mln::rgb8{0, 0, 0}).build();
  // for()
}
