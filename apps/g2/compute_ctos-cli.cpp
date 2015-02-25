#include <mln/core/image/image2d.hpp>
#include <mln/core/colors.hpp>
#include <mln/morpho/component_tree/io.hpp>
#include <mln/morpho/component_tree/compute_depth.hpp>
#include <mln/morpho/component_tree/reconstruction.hpp>
#include "compute_ctos.hpp"
#include <apps/tos/croutines.hpp>

int main(int argc, char** argv)
{
  using namespace mln;


  if (argc < 3)
    {
      std::cerr << "Usage: " << argv[0] << " input(color) out.tree [out.tiff] [grain]\n"
        "Compute the cToS of an image. It ouputs the tree (out.tree)"
        "and an optional depth image (out.tiff). The tree is computed on "
        "K1 (doubled-sized image + border) and so is the depth image\n"
        "If a grain is given, a grain filter is applied.\n";
      std::exit(1);
    }

  image2d<rgb8> f;
  io::imread(argv[1], f);

  auto tree = compute_ctos(f, NULL);

  if (argc > 4)
    grain_filter_inplace(tree, std::atoi(argv[4]));

  morpho::save(tree, argv[2]);

  if (argc > 3) {
    image2d<uint16> depth;
    depth.resize(tree._get_data()->m_pmap.domain());
    auto dmap = morpho::compute_depth(tree);
    morpho::reconstruction(tree, dmap, depth);
    io::imsave(depth, argv[3]);
  }
}
