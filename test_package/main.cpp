#include <mln/core/algorithm/iota.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/io/imprint.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>


int main()
{
  mln::image2d<uint8_t> f(5, 5);

  mln::iota(f, 0);

  mln::io::imprint(f);
  mln::io::imsave(f, "test.pgm");

  mln::box2d b;
  b.x();

  mln::io::imread("test.pgm", f);
}
