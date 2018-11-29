#include "tos_tests_helper.hpp"
#include <mln/core/image/image2d.hpp>
#include <mln/core/image/image3d.hpp>
#include <mln/morpho/component_tree/component_tree.hpp>
#include <mln/morpho/component_tree/reconstruction.hpp>


template <class ParentImageType>
ParentImageType
    tree_as_parent_image(const mln::morpho::component_tree<unsigned, mln_ch_value(ParentImageType, unsigned)>& tree,
                         ParentImageType& rootimage)
{
  using tree_t = std::decay_t<decltype(tree)>;

  const auto& pmap = tree._get_data()->m_pmap;

  // Store the minimal index for each component
  using P                            = mln_point(ParentImageType);
  constexpr P                  UNDEF = mln::value_traits<P>::inf();
  mln::property_map<tree_t, P> roots(tree, UNDEF);
  mln_foreach (auto px, pmap.pixels())
  {
    auto  node_id = px.val();
    auto& r       = roots[node_id];
    if (r == UNDEF)
      r = px.point();
  }

  mln::resize(rootimage, pmap);
  mln::morpho::reconstruction(tree, roots, rootimage);

  auto parent = mln::clone(rootimage);
  mln_foreach (auto node, tree.nodes_without_root())
    parent(roots[node.id()]) = roots[node.get_parent_id()];

  return parent;
}

using tree_2d_t = mln::morpho::component_tree<unsigned, mln::image2d<unsigned>>;
using par_2d_t  = mln::image2d<mln::point2d>;
using tree_3d_t = mln::morpho::component_tree<unsigned, mln::image3d<unsigned>>;
using par_3d_t  = mln::image3d<mln::point3d>;


// Explicit instanciation
template par_2d_t tree_as_parent_image(const tree_2d_t& tree, par_2d_t& roots);

template par_3d_t tree_as_parent_image(const tree_3d_t& tree, par_3d_t& roots);
