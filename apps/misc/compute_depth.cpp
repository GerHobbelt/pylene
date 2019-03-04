#include <mln/core/image/image2d.hpp>
#include <mln/core/image/morphers/casted_image.hpp>
#include <mln/io/imsave.hpp>
#include <mln/morpho/component_tree/compute_depth.hpp>
#include <mln/morpho/component_tree/io.hpp>
#include <mln/morpho/component_tree/reconstruction.hpp>


int main(int argc, char** argv)
{
  if (argc < 3)
  {
    std::cerr << "Usage: " << argv[0] << " input.tree outdepth.tiff\n";
    std::exit(1);
  }


  using namespace mln;

  morpho::component_tree<unsigned, image2d<unsigned>> tree;

  morpho::load(argv[1], tree);
  auto dmap = morpho::compute_depth(tree);

  image2d<unsigned> out;
  resize(out, tree._get_data()->m_pmap);
  morpho::reconstruction(tree, dmap, out);

  io::imsave(imcast<uint16>(out), argv[2]);
}
