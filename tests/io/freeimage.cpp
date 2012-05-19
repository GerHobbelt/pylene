#include <mln/core/ndimage.hpp>
#include <mln/core/grays.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imprint.hpp>
#include <mln/io/freeimage_reader.hpp>


int main(int argc, char** argv)
{
  assert(argc > 1);
  const char* path = argv[1];

  mln::image2d<mln::uint8> ima;
  mln::io::imread(path, mln::io::freeimage_reader (), ima);

  mln::io::imprint(ima);
}
