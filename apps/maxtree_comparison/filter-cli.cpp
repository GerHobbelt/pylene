#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>

int main(int argc, char** argv)
{

  if (argc < 6)
  {
    std::cerr << "Usage: " << argv[0]
              << " original parent order lambda out\n"
                 "original: original image\n"
                 "parent: parent image\n"
                 "order:  order image\n"
                 "lambda: area filter size\n"
                 "out:    output image\n";
    std::exit(1);
  }

  using namespace mln;
  typedef uint8 V;

  image2d<V>        f(0);
  image2d<unsigned> par(0);
  image2d<unsigned> S(0);

  io::imread(argv[1], f);
  io::imread(argv[2], par);
  io::imread(argv[3], S);

  image2d<unsigned> area;
  resize(area, f).set_init_value(0);

  // Accumulate
  mln_reverse_foreach (unsigned i, S.values())
  {
    area[i]++;
    area[par[i]] += area[i];
  }

  unsigned lambda = std::atoi(argv[4]);
  mln_foreach (unsigned i, S.values())
  {
    if (area[i] < lambda)
      f[i] = f[par[i]];
  }

  io::imsave(f, argv[5]);
}
