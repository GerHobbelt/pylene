#include <mln/core/algorithm/iota.hpp>
#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/io/experimental/imprint.hpp>
#include <mln/io/experimental/imread.hpp>
#include <mln/io/experimental/imsave.hpp>


int main()
{
  mln::experimental::image2d<uint8_t> f(5, 5);

  mln::iota(f, 0);

  mln::io::experimental::imprint(f);
  mln::io::experimental::imsave(f, "test.pgm");

  mln::experimental::box2d b;
  b.x();

  mln::io::experimental::imread("test.pgm", f);
}
