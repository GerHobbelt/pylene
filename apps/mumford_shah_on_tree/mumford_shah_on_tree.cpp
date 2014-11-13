#include <mln/core/image/image2d.hpp>
#include <mln/morpho/component_tree/io.hpp>

#include <mln/accu/accumulators/variance.hpp>
#include <mln/accu/accumulators/accu_as_it.hpp>

#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>

#include <apps/tos/Kinterpolate.hpp>
#include "mumford_shah_on_tree.hpp"

typedef mln::morpho::component_tree<unsigned, mln::image2d<unsigned> > tree_t;

tree_t::vertex_id_t
find_canonical(mln::property_map<tree_t, tree_t::vertex_id_t>& newpar,
               tree_t::vertex_id_t x)
{
  if (newpar[x] == tree_t::npos()) // x is alive
    return x;
  else
    return newpar[x] = find_canonical(newpar, newpar[x]);
}


int main(int argc, char** argv)
{
  if (argc < 6) {
    std::cerr << "Usage: " << argv[0] << " input.ppm input.tree lambda grain output.ppm\n"
      ;
    std::exit(1);
  }


  const char* input_path = argv[1];
  const char* tree_path = argv[2];
  float lambda = std::atof(argv[3]);
  unsigned grain = std::atoi(argv[4]);
  const char* output_path = argv[5];


  using namespace mln;



  image2d<rgb8> f, F;
  tree_t tree;

  io::imread(input_path, f);
  morpho::load(tree_path, tree);

  F = Kadjust_to(f, tree._get_data()->m_pmap.domain());

  // Filter the grain first
  {
    accu::accumulators::accu_if< accu::accumulators::count<>,
                                 K1::is_face_2_t,
                                 point2d > counter;

    auto areamap = morpho::paccumulate(tree, F, counter);
    auto pred = make_functional_property_map<tree_t::vertex_id_t>([&areamap, grain](tree_t::vertex_id_t x) {
        return areamap[x] > grain;
      });
    morpho::filter_direct_inplace(tree, pred);
    tree.shrink_to_fit();
  }

  mumford_shah_on_tree(tree, F, lambda);
  F = Kadjust_to(F, f.domain());
  io::imsave(F, output_path);
}
