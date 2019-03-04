#include "circularity.hpp"

#include <mln/accu/accumulators/covariance.hpp>
#include <mln/morpho/component_tree/accumulate.hpp>


mln::property_map<tree_t, float> circularity(const tree_t& tree)
{
  using namespace mln;

  mln_entering("compute_circularity");

  accu::accumulators::covariance<point2d, double> COV;

  auto covmap = morpho::paccumulate(tree, tree._get_data()->m_pmap, COV);


  property_map<tree_t, float> vmap(tree);

  mln_foreach (auto x, tree.nodes())
  {
    auto   cov = covmap[x];
    double T   = cov(0, 0) + cov(1, 1);
    double D   = cov(0, 0) * cov(1, 1) - cov(0, 1) * cov(1, 0);
    double tmp = sqrt(T * T / 4 - D);
    double l1, l2;

    if (tmp > 0)
    {
      l1 = T / 2 + tmp;
      l2 = T / 2 - tmp;
    }
    else
    {
      l2 = T / 2 + tmp;
      l1 = T / 2 - tmp;
    }

    vmap[x] = 1 - l2 / l1;
  }

  mln_exiting();
  return vmap;
}
