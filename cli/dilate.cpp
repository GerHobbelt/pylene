#include <mln/core/image/image2d.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>

#include <iostream>
#include <mln/core/se/disc.hpp>
#include <mln/core/win2d.hpp>
#include <mln/morpho/structural/dilate.hpp>

int main(int argc, char** argv)
{
  using namespace mln;

  if (argc < 4)
  {
    std::cerr << "Usage: " << argv[0] << " input ball_radius output(gray)" << std::endl;
    std::exit(1);
  }

  image2d<uint8> ima;
  io::imread(argv[1], ima);

  int  radius = std::atoi(argv[2]);
  auto w      = se::disc(radius + 0.3f);

  image2d<uint8> out = morpho::structural::dilate(ima, w);
  io::imsave(out, argv[3]);
}
