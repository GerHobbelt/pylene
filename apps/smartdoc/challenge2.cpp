#include "border_detection.hpp"
#include "unhomo.hpp"
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>



int main(int argc, char** argv)
{
  if (argc < 3)
    {
      std::cerr << "Usage: " << argv[0] << " input.jpg output.jpg [detection.jpg]\n"
        ;
      std::exit(1);
    }

  using namespace mln;


  image2d<rgb8> f, out;
  io::imread(argv[1], f);

  const char* debug_filename = (argc > 3) ? argv[3] : NULL;

  pquad_t pts = border_detection(f, debug_filename);
  out = unhomo(f, pts);

  io::imsave(out, argv[2]);
}

