#include "tos_tests_helper.hpp"
#include <mln/morpho/component_tree/component_tree.hpp>
#include <mln/morpho/component_tree/reconstruction.hpp>


mln::image2d<mln::point2d>
tree_as_parent_image(const mln::morpho::component_tree<unsigned, mln::image2d<unsigned>>& tree,
                     mln::image2d<mln::point2d>& rootimage)
{
  using tree_t = mln::morpho::component_tree<unsigned, mln::image2d<unsigned>>;

  const auto& pmap = tree._get_data()->m_pmap;


  // Store the minimal index for each component
  constexpr mln::point2d UNDEF = mln::value_traits<mln::point2d>::inf();
  mln::property_map<tree_t, mln::point2d> roots(tree, UNDEF);
  mln_foreach(auto px, pmap.pixels())
  {
    auto node_id = px.val();
    auto& r = roots[node_id];
    if (r == UNDEF)
      r = px.point();
  }

  mln::resize(rootimage, pmap);
  mln::morpho::reconstruction(tree, roots, rootimage);

  auto parent = mln::clone(rootimage);
  mln_foreach(auto node, tree.nodes_without_root())
    parent(roots[node.id()]) = roots[node.get_parent_id()];

  return parent;
}
