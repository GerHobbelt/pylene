#include <mln/core/colors.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/views.hpp>
#include <mln/io/experimental/imread.hpp>
#include <mln/io/experimental/imsave.hpp>


int main()
{
  auto  in     = mln::io::experimental::imread("images/lena_color.png");
  auto* in_ptr = in.cast_to<mln::rgb8, 2>();

  mln::io::experimental::imsave(mln::view::red(*in_ptr), "images/lena_red_mono.png");
  mln::io::experimental::imsave(mln::view::green(*in_ptr), "images/lena_green_mono.png");
  mln::io::experimental::imsave(mln::view::blue(*in_ptr), "images/lena_blue_mono.png");

  auto filter_red   = [](auto v) -> mln::rgb8 { return {v[0], 0, 0}; };
  auto filter_green = [](auto v) -> mln::rgb8 { return {0, v[1], 0}; };
  auto filter_blue  = [](auto v) -> mln::rgb8 { return {0, 0, v[2]}; };

  mln::io::experimental::imsave(mln::view::transform(*in_ptr, filter_red), "images/lena_red.png");
  mln::io::experimental::imsave(mln::view::transform(*in_ptr, filter_green), "images/lena_green.png");
  mln::io::experimental::imsave(mln::view::transform(*in_ptr, filter_blue), "images/lena_blue.png");
}
