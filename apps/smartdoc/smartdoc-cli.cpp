#include <mln/core/image/image2d.hpp>
#include <mln/morpho/component_tree/io.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include "smartdoc.hpp"




int main(int argc, char** argv)
{
  if (argc < 6)
    {
      std::cerr << "Usage: " << argv[0] << "input.tree input.tiff out.tree out.csv out.tiff\n"
        ;
      std::exit(1);
    }

  using namespace mln;


  tree_t tree;
  morpho::load(argv[1], tree);

  image2d<uint16> depth;
  io::imread(argv[2], depth);

  image2d<rgb8> out;
  std::array<process_result_t, 3> res = process(tree, depth, argv[4], argv[3]);

  resize(out, depth);
  draw_quad_superimpose(res[0].points, depth, out);

  io::imsave(out, argv[5]);
}
