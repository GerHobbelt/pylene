#include <mln/core/image/image2d.hpp>
#include <mln/core/neighb2d.hpp>
#include <mln/morpho/maxtree_pqueue_parallel.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>



int main(int argc, char** argv)
{
  if (argc < 5) {
    std::cerr << "Usage: " << argv[0] << " input (4|8) parent.tiff order.tiff\n"
      "input: input image (32bit unsigned)\n"
      "(4|8): connexity (either 4 or 8)\n"
      "parent.tiff: output parent image\n"
      "order.tiff: output order image\n";
    std::exit(1);
  }

  using namespace mln;

  typedef uint8 V;
  image2d<V> f;
  io::imread(argv[1], f);


  image2d<unsigned>     parent;
  std::vector<unsigned> S;

  int connexity = std::atoi(argv[2]);
  if (connexity == 4) {
    std::tie(parent, S) = morpho::impl::parallel::maxtree_pqueue(f, c4);
  } else if (connexity == 8) {
    std::tie(parent, S) = morpho::impl::parallel::maxtree_pqueue(f, c8);
  } else {
    std::cerr << "Connexity must be 4 or 8\n";
    std::exit(1);
  }

  unsigned nr = f.nrows();
  unsigned nc = f.ncols();

  image2d<unsigned> par(nr, nc, 0);
  image2d<unsigned> order(nr, nc, 0);

  {
    point2d p;
    {
      mln_iter(vin, parent.values());
      mln_iter(vout, par.values());
      mln_forall(vin, vout) {
        p = f.point_at_index(*vin);
        *vout = p[0] * nc + p[1];
      };
    }
    {
      mln_iter(vin, S);
      mln_iter(vout, order.values());
      mln_forall(vin, vout) {
        p = f.point_at_index(*vin);
        *vout = p[0] * nc + p[1];
      }
    }
  }

  io::imsave(par, argv[3]);
  io::imsave(order, argv[4]);
}

