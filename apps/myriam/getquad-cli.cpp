#include <mln/core/image/image2d.hpp>
#include <mln/io/imread.hpp>
#include "getquad.hpp"


int main(int argc, char** argv)
{
  using namespace mln;

  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " input(binary).tiff output.png\n";
    std::exit(1);
  }


  image2d<bool> f;
  io::imread(argv[1], f);

  getquad(f, argv[2]);
}
