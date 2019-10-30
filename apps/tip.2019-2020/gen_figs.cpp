#include <mln/core/image/image2d.hpp>
#include <mln/core/image/views.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/grays.hpp>
#include <mln/core/colors.hpp>
#include <mln/io/experimental/imread.hpp>
#include <mln/io/experimental/imsave.hpp>


int main()
{
  auto in = mln::io::experimental::imread("images/lena_color.png");
  auto* in_ptr = in.cast_to<mln::rgb8, 2>();

  mln::io::experimental::imsave(mln::view::green(*in_ptr), "images/lena_green.png");
  mln::io::experimental::imsave(mln::view::red(*in_ptr), "images/lena_red.png");
  mln::io::experimental::imsave(mln::view::blue(*in_ptr), "images/lena_blue.png");
}
