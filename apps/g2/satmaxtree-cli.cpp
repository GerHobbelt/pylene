#include <mln/core/image/image2d.hpp>
#include <mln/io/imread.hpp>
#include <mln/io/imsave.hpp>

#include <mln/morpho/component_tree/accumulate.hpp>
#include <mln/morpho/component_tree/compute_depth.hpp>
#include <mln/morpho/component_tree/reconstruction.hpp>
#include <mln/morpho/component_tree/io.hpp>
#include <apps/tos/Kinterpolate.hpp>
#include <apps/tos/topology.hpp>
#include "satmaxtree.hpp"

namespace mln
{

  template <class P>
  morpho::component_tree<P, image2d<P> >
  tree_keep_2F(const morpho::component_tree<P, image2d<P> >& tree)
  {
    morpho::component_tree<P, image2d<P> > out;

    auto newdata = out._get_data();
    auto olddata = tree._get_data();

    // 1. Copy the point2node map
    box2d olddom = olddata->m_pmap.domain();
    box2d dom;
    dom.pmin = olddom.pmin / 2;
    dom.pmax = (olddom.pmax + 1) / 2;
    newdata->m_pmap.resize(dom);
    copy(olddata->m_pmap | sbox2d{olddom.pmin, olddom.pmax, {2,2}},
         newdata->m_pmap);

    // 2. Copy the node
    newdata->m_nodes = olddata->m_nodes;

    // 3. Copy the point set and update node first point index/
    newdata->m_S.resize(dom.size());
    unsigned j = 0;
    for (unsigned i = 0; i < olddata->m_S.size(); ++i)
      {
        P p = olddata->m_S[i];
        point2d pt = olddata->m_pmap.point_at_index(p);
        if (K1::is_face_2(pt))
          {
            newdata->m_S[j] = newdata->m_pmap.index_of_point(pt/2);
            auto node = tree.get_node_at(p);
            if (node.get_first_point_id() == i)
              newdata->m_nodes[node.id()].m_point_index = j;
            ++j;
          }
      }
    // 4. Do not forget the sentinel
    newdata->m_nodes[out.npos()].m_point_index = j;

    return out.get_subtree(tree.get_root_id());
  }

}

void usage(char** argv)
{
  std::cerr << "Usage: " << argv[0] << "(K0|K1) depth.tiff out.tree out.tiff [colorimg]\n"
    "Compute the saturated max tree from a depth 16-bit image. \n"
    "It outputs the tree and the new depth image that matches this tree. \n"
    "If K0, the tree+output has the same domain as depth (with 0-1 faces removed),"
    "otherwise it outputs 0 and 1F as well.\n";
  std::exit(1);
}

int main(int argc, char** argv)
{
  if (argc < 5)
    usage(argv);

  using namespace mln;

  std::string topo = argv[1];
  image2d<uint16> depth;
  io::imread(argv[2], depth);

  typedef morpho::component_tree<unsigned, image2d<unsigned> > tree_t;
  tree_t dtree;
  property_map<tree_t, uint16> vmap;
  box2d domain;

  std::tie(dtree, vmap) = satmaxtree(depth);
  tree_t tree;
  if (topo == "K0") {
    tree = tree_keep_2F(dtree);
    domain = depth.domain();
    // depth = depth;
  } else if (topo == "K1") {
    tree = dtree;
    domain = depth.domain();
    domain.pmax = domain.pmax * 2 - 1;
  } else {
    usage(argv);
  }

  //auto vmap = morpho::make_attribute_map_from_image(tree, depth);
  //auto vmap = morpho::compute_depth(tree);

  image2d<uint16> out(domain);
  morpho::reconstruction(tree, vmap, out);

  // image2d<uint16> out;
  // resize(out, depth);
  // copy(tmp | sbox2d{ {0,0}, domain.pmax, {2,2} }, out);
  // io::imsave(out, argv[2]);
  {
    std::ofstream f(argv[3]);
    morpho::save(tree, f);
  }
  io::imsave(out, argv[4]);
}




