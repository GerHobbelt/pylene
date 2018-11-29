#include "compute_ctos.hpp"
#include <mln/core/colors.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/morpho/component_tree/io.hpp>

int main(int argc, char** argv)
{
  using namespace mln;

  if (argc < 3)
  {
    std::cerr << "Usage: " << argv[0]
              << " input(color) out.tree [out.tiff]\n"
                 "Compute the cToS of an image. It ouputs the tree (out.tree)"
                 "and an optional depth image (out.tiff). The tree is computed on"
                 "K1 (doubled-sized image + border) and so is the depth image";
    std::exit(1);
  }

  image2d<rgb8> f;
  io::imread(argv[1], f);

  image2d<uint16>  depth;
  image2d<uint16>* depthptr = (argc > 3) ? &depth : NULL;

  auto tree = compute_ctos(f, depthptr);

  morpho::save(tree, argv[2]);
  if (argc > 3)
    io::imsave(depth, argv[3]);
}
