#include <mln/core/neighb2d.hpp>
#include <mln/morpho/tos/ctos.hpp>
#include <mln/morpho/component_tree/accumulate.hpp>
#include <mln/morpho/component_tree/filtering.hpp>
#include <apps/tos/Kinterpolate.hpp>
#include "satmaxtree.hpp"

namespace mln
{

  std::pair<
    morpho::component_tree<unsigned, image2d<unsigned> >,
    property_map<morpho::component_tree<unsigned, image2d<unsigned> >, uint16>
    >
  satmaxtree(const image2d<uint16>& f)
  {
    mln_entering("mln::satmaxtree");

    typedef morpho::component_tree<unsigned, image2d<unsigned> > T;

    T tree = morpho::cToS(f, c4);
    image2d<uint16> F = immerse_k1(f, 69);
    property_map<T, uint16> vmap = morpho::make_attribute_map_from_image(tree, F);
    vmap[tree.npos()] = 0;

    auto predfun = [&vmap,&tree](const T::vertex_id_t& n) {
      return vmap[n] > vmap[tree.get_node(n).parent()] or tree.get_node(n).get_parent_id() == tree.npos();
    };
    auto pred = make_functional_property_map<T::vertex_id_t> (predfun);

    // Subtractive
    {
      property_map<T, unsigned> delta(tree, 0);
      unsigned n = 0;
      mln_foreach(auto x, tree.nodes_without_root()) {
        delta[x] = delta[x.parent()];
        if (vmap[x] < vmap[x.parent()])
          delta[x] += vmap[x.parent()] - vmap[x];
        ++n;
      }
      // We need to separate the two loops !
      mln_foreach(auto x, tree.nodes_without_root()) {
        vmap[x] += delta[x];
      }
      std::cout << "Number of nodes before: " << n << std::endl;
    }

    morpho::filter_direct_inplace(tree, pred);
    {
      unsigned n = 0;
      mln_foreach(auto x, tree.nodes()) {
        (void) x;
        assert(vmap[x] > vmap[x.parent()]);
        ++n;
      }
      std::cout << "Number of nodes after: " << n << std::endl;
    }

    mln_exiting();
    return {std::move(tree), std::move(vmap)};
  }

}
