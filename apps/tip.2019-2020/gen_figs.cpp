#include <mln/core/algorithm/all_of.hpp>
#include <mln/core/algorithm/clone.hpp>
#include <mln/core/algorithm/copy.hpp>
#include <mln/core/algorithm/fill.hpp>
#include <mln/core/colors.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/views.hpp>
#include <mln/io/experimental/imread.hpp>
#include <mln/io/experimental/imsave.hpp>

#include <cmath>
#include <iostream>


int main()
{
  mln::experimental::image2d<mln::rgb8> lena_color;
  mln::io::experimental::imread("images/lena_color.png", lena_color);


  mln::io::experimental::imsave(mln::view::red(lena_color), "images/lena_red_mono.png");
  mln::io::experimental::imsave(mln::view::green(lena_color), "images/lena_green_mono.png");
  mln::io::experimental::imsave(mln::view::blue(lena_color), "images/lena_blue_mono.png");

  auto filter_red   = [](auto v) -> mln::rgb8 { return {v[0], 0, 0}; };
  auto filter_green = [](auto v) -> mln::rgb8 { return {0, v[1], 0}; };
  auto filter_blue  = [](auto v) -> mln::rgb8 { return {0, 0, v[2]}; };

  auto lena_red   = mln::view::transform(lena_color, filter_red);
  auto lena_green = mln::view::transform(lena_color, filter_green);
  auto lena_blue  = mln::view::transform(lena_color, filter_blue);

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

  if (mln::all_of(lena_color - lena_red == lena_gb))
  {
    std::cout << "rgb - red == green + blue" << std::endl;
  }
  if (mln::all_of(lena_color - lena_green == lena_rb))
  {
    std::cout << "rgb - green == red + blue" << std::endl;
  }
  if (mln::all_of(lena_color - lena_blue == lena_rg))
  {
    std::cout << "rgb - blue == red + green" << std::endl;
  }

  mln::io::experimental::imsave(lena_red + lena_green, "images/lena_rg.png");
  mln::io::experimental::imsave(lena_red + lena_blue, "images/lena_rb.png");
  mln::io::experimental::imsave(lena_green + lena_blue, "images/lena_bg.png");

  mln::experimental::image2d<mln::rgb8> planet_color;
  mln::io::experimental::imread("images/planet.png", planet_color);

  auto lena_plus_planet = lena_color + planet_color;
  mln::io::experimental::imsave(lena_plus_planet, "images/lena_+_planet.png");

  auto out        = lena_color.concretize().set_init_value(mln::rgb8{0, 0, 0}).build();
  auto zipped_ima = mln::view::zip(out, lena_red, lena_blue);

  auto zipped_pixels = zipped_ima.new_pixels();
  for (auto&& row : mln::ranges::rows(zipped_pixels))
  {
    for (auto&& zpix : row)
    {
      auto&& [oval, rval, bval] = zpix.val();
      oval                      = rval + bval;
    }
  }
  mln::io::experimental::imsave(out, "images/lena_rb2.png");

  if (mln::all_of(lena_rb == out))
  {
    std::cout << "zipped out == red + blue" << std::endl;
  }

  {
    float gamma                      = 2.5f;
    auto  gamma_fun                  = [gamma](mln::rgb8 x) -> mln::rgb8 { return 256.f * pow(x / 256.f, 1 / gamma); };
    auto  lena_color_gamma_corrected = mln::view::transform(lena_color, gamma_fun);
    mln::io::experimental::imsave(lena_color_gamma_corrected, "images/lena_color_gamma_corrected.png");
  }

  {
    mln::experimental::image2d<mln::uint8> lena_grey;
    mln::io::experimental::imread("images/lena_grey.png", lena_grey);

    float threshold        = 150;
    auto  binary_fun       = [threshold](auto x) -> mln::uint8 { return (x < threshold) ? 0 : 255; };
    auto  lena_grey_binary = mln::view::transform(lena_grey, binary_fun);
    mln::io::experimental::imsave(lena_grey_binary, "images/lena_grey_binary.png");

    float gamma                     = 2.5f;
    auto  gamma_fun                 = [gamma](auto x) -> mln::uint8 { return 256.f * pow(x / 256.f, 1 / gamma); };
    auto  lena_grey_gamma_corrected = mln::view::transform(lena_grey, gamma_fun);
    mln::io::experimental::imsave(lena_grey_gamma_corrected, "images/lena_grey_gamma_corrected.png");
  }

  {
    mln::experimental::image2d<mln::rgb8> lena_color;
    mln::io::experimental::imread("images/lena_color.png", lena_color);

    auto lena_color_filtered =
        mln::view::filter(lena_color, [](auto v) -> bool { return (v[0] + v[1] + v[2]) % 2 == 0; });
    fill(lena_color_filtered, mln::rgb8{255, 255, 255});
    mln::io::experimental::imsave(lena_color, "images/lena_color_filtered.png");
  }


  {
    mln::experimental::image2d<mln::uint8> lena_grey;
    mln::io::experimental::imread("images/lena_grey.png", lena_grey);

    float threshold                 = 150;
    auto  binary_fun                = [threshold](auto x) -> bool { return x < threshold; };
    auto  lena_grey_binary_filtered = mln::view::filter(lena_grey, binary_fun);
    fill(lena_grey_binary_filtered, mln::uint8(0));
    mln::io::experimental::imsave(lena_grey, "images/lena_grey_binary_filtered.png");
  }

  {
    mln::experimental::image2d<mln::uint8> lena_grey;
    mln::io::experimental::imread("images/lena_grey.png", lena_grey);

    auto dom     = lena_grey.domain();
    auto tl      = mln::experimental::point2d{dom.width() / 4, dom.height() / 4};
    auto br      = mln::experimental::point2d{dom.width() / 4 + dom.width() / 2, dom.height() / 4 + dom.width() / 2};
    auto sub_dom = mln::experimental::box2d{tl, br};
    auto sub_lena_grey = mln::view::clip(lena_grey, sub_dom);
    mln::io::experimental::imsave(sub_lena_grey, "images/lena_grey_clipped.png");

    fill(sub_lena_grey, mln::uint8(255));
    mln::io::experimental::imsave(lena_grey, "images/lena_grey_minus_clipped.png");
  }

  {
    mln::experimental::image2d<mln::rgb8> lena_color;
    mln::io::experimental::imread("images/lena_color.png", lena_color);

    auto dom     = lena_color.domain();
    auto tl      = mln::experimental::point2d{dom.width() / 4, dom.height() / 4};
    auto br      = mln::experimental::point2d{dom.width() / 4 + dom.width() / 2, dom.height() / 4 + dom.width() / 2};
    auto sub_dom = mln::experimental::box2d{tl, br};
    auto sub_lena_color = mln::view::clip(lena_color, sub_dom);
    mln::io::experimental::imsave(sub_lena_color, "images/lena_color_clipped.png");

    fill(sub_lena_color, mln::rgb8{255, 255, 255});

    mln::experimental::image2d<mln::rgb8> planet_color;
    mln::io::experimental::imread("images/planet.png", planet_color);

    auto dom2 = planet_color.domain();
    auto tl2  = mln::experimental::point2d{dom2.width() / 4, dom2.height() / 4};
    auto br2  = mln::experimental::point2d{dom2.width() / 4 + dom2.width() / 2, dom2.height() / 4 + dom2.width() / 2};
    auto sub_dom2         = mln::experimental::box2d{tl2, br2};
    auto sub_planet_color = mln::view::clip(planet_color, sub_dom2);
    mln::io::experimental::imsave(sub_planet_color, "images/planet_clipped.png");

    auto sub_planet_color_clone = clone(sub_planet_color);
    fill(planet_color, mln::rgb8{0, 0, 0});
    copy(sub_planet_color_clone, sub_planet_color);
    mln::io::experimental::imsave(planet_color, "images/planet_minus_clipped.png");

    mln::io::experimental::imsave(lena_color + planet_color, "images/lena_+_planet_color.png");
  }

  {
    mln::experimental::image2d<mln::rgb8> planet_color;
    mln::io::experimental::imread("images/planet.png", planet_color);

    auto dom     = planet_color.domain();
    auto tl      = mln::experimental::point2d{dom.width() / 4, dom.height() / 4};
    auto br      = mln::experimental::point2d{dom.width() / 4 + dom.width() / 2, dom.height() / 4 + dom.width() / 2};
    auto sub_dom = mln::experimental::box2d{tl, br};
    auto sub_planet_color = mln::view::clip(planet_color, sub_dom);

    fill(sub_planet_color, mln::rgb8{255, 255, 255});

    mln::experimental::image2d<mln::rgb8> lena_color;
    mln::io::experimental::imread("images/lena_color.png", lena_color);

    auto dom2 = lena_color.domain();
    auto tl2  = mln::experimental::point2d{dom2.width() / 4, dom2.height() / 4};
    auto br2  = mln::experimental::point2d{dom2.width() / 4 + dom2.width() / 2, dom2.height() / 4 + dom2.width() / 2};
    auto sub_dom2       = mln::experimental::box2d{tl2, br2};
    auto sub_lena_color = mln::view::clip(lena_color, sub_dom2);

    auto sub_lena_color_clone = clone(sub_lena_color);
    fill(lena_color, mln::rgb8{0, 0, 0});
    copy(sub_lena_color_clone, sub_lena_color);
    mln::io::experimental::imsave(lena_color, "images/lena_color_minus_clipped.png");

    mln::io::experimental::imsave(lena_color + planet_color, "images/lena_+_planet_color2.png");
  }

  {
    mln::experimental::image2d<mln::uint8> lena_grey;
    mln::io::experimental::imread("images/lena_grey.png", lena_grey);

    auto lena_grey_masked = mln::view::mask(lena_grey, lena_grey > 150);
    fill(lena_grey_masked, mln::uint8(255));

    mln::io::experimental::imsave(lena_grey, "images/lena_grey_masked1.png");
  }

  {
    mln::experimental::image2d<mln::uint8> lena_grey;
    mln::io::experimental::imread("images/lena_grey.png", lena_grey);

    auto lena_grey_masked2 = mln::view::mask(lena_grey, lena_grey <= 150);
    fill(lena_grey_masked2, mln::uint8(0));

    mln::io::experimental::imsave(lena_grey, "images/lena_grey_masked2.png");
  }

  {
    mln::experimental::image2d<mln::uint8> lena_grey;
    mln::io::experimental::imread("images/lena_grey.png", lena_grey);

    auto lena_grey_masked = mln::view::mask(lena_grey, lena_grey > 150);
    fill(lena_grey_masked, mln::uint8(255));

    auto lena_grey_masked2 = mln::view::mask(lena_grey, lena_grey <= 150);
    fill(lena_grey_masked2, mln::uint8(0));

    mln::io::experimental::imsave(lena_grey, "images/lena_grey_binary.png");
  }
}
