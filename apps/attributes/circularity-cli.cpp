#include "attributes.hpp"
#include "circularity.hpp"

#include <apps/tos/Kinterpolate.hpp>
#include <apps/tos/croutines.hpp>
#include <apps/tos/topology.hpp>

#include <mln/accu/accumulators/mean.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>
#include <mln/morpho/component_tree/accumulate.hpp>
#include <mln/morpho/component_tree/reconstruction.hpp>


int main(int argc, char** argv)
{
  using namespace mln;

  po::options_description desc("Circularity Options");
  desc.add_options()("export-rec", po::value<std::string>(), "For reconstruction.");

  // desc.add_options()
  //   ("eps,h", po::value<float>()->required(), "The height for neighbor lookup (e.g. 10).")
  //   ("export-rec", po::value<std::string>(), "For reconstruction.");

  po::variables_map vm   = process_cmdline(argc, argv, desc);
  tree_t            tree = preprocess(vm);

  image2d<rgb8> f;
  io::imread(vm["input_path"].as<std::string>(), f, true);

  image2d<rgb8> F = Kadjust_to(f, tree._get_data()->m_pmap.domain());

  auto perimeter = compute_attribute_on_contour(tree, F, accu::features::count<>());
  auto area      = morpho::accumulate(tree, accu::features::count<>());
  auto circu     = circularity(tree);

  constexpr float coefnorm = 2 * 1.7724538509055159; // 2.(pi^.5)

  property_map<tree_t, float> vmap(tree);
  mln_foreach (auto x, tree.nodes())
  {
    vmap[x] = perimeter[x] / (coefnorm * std::sqrt(area[x])) + circu[x];
  }

  auto smap = postprocess(vm, tree, vmap);

  auto pa = make_functional_property_map<tree_t::node_type>(
      [&area, &perimeter](const tree_t::node_type& x) { return perimeter[x] / (coefnorm * std::sqrt(area[x])); });

  const char*                             names[] = {"Circularity", "P/A", "Energy", "Extinction"};
  std::function<float(tree_t::node_type)> attrs[] = {_as_fun(circu), _as_fun(pa), _as_fun(vmap), _as_fun(smap)};

  export_(vm, tree, smap, names, attrs, 4);

  if (vm.count("export-rec"))
  {
    auto vmap = morpho::vaccumulate_proper(tree, F, accu::features::mean<>());
    morpho::reconstruction(tree, vmap, F);
    io::imsave(F, vm["export-rec"].as<std::string>());
  }
}
