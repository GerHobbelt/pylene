#include <apps/tos/Kinterpolate.hpp>
#include <apps/tos/topology.hpp>

#include <mln/accu/accumulators/accu_if.hpp>
#include <mln/accu/accumulators/count.hpp>
#include <mln/accu/accumulators/mean.hpp>
#include <mln/core/image/image2d.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <mln/morpho/component_tree/accumulate.hpp>
#include <mln/morpho/component_tree/filtering.hpp>
#include <mln/morpho/component_tree/io.hpp>
#include <mln/morpho/component_tree/reconstruction.hpp>


int main(int argc, char** argv)
{
  using namespace mln;

  if (argc < 5)
  {
    std::cerr << "Usage: " << argv[0]
              << " input(.jpg|.png) tree lambda output(.jpg|.png)\n"
                 "Note: Non-filtered pixels are left with their original color. Filtered pixels"
                 "are set to the average color of the least survival node."
              << std::endl;
    std::exit(1);
  }

  typedef morpho::component_tree<unsigned, image2d<unsigned>> tree_t;

  image2d<rgb8> f;
  tree_t        tree;
  unsigned      lambda;

  io::imread(argv[1], f);
  morpho::load(argv[2], tree);
  lambda = std::atoi(argv[3]);

  image2d<rgb8> F = Kadjust_to(f, tree._get_data()->m_pmap.domain());

  auto isface2 = [](const point2d& p) { return K1::is_face_2(p); };
  typedef accu::accumulators::accu_if<accu::accumulators::count<>, decltype(isface2), point2d> ACC;
  ACC accu(accu::accumulators::count<>(), isface2);

  auto areamap = morpho::paccumulate(tree, F, accu);

  auto vmap = morpho::vaccumulate_proper(tree, F, accu::features::mean<>());
  std::cout << "Grain size: " << lambda << std::endl;

  mln_foreach (auto x, tree.nodes())
    if (areamap[x] < lambda)
      vmap[x] = vmap[x.parent()];

  mln_foreach (auto px, F.pixels())
  {
    tree_t::node_type x = tree.get_node_at(px.index());
    if (areamap[x] < lambda)
      px.val() = vmap[x];
  }

  F = Kadjust_to(F, f.domain());
  io::imsave(F, argv[4]);
}
