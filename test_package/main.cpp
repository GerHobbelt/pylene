#include <mln/core/algorithm/iota.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/io/imprint.hpp>
#include <mln/io/imsave.hpp>


int main()
{
  mln::image2d<mln::uint8> f(5, 5);

  mln::iota(f, 0);

  mln::io::imprint(f);
  mln::io::imsave(f, "test.pgm");
}
