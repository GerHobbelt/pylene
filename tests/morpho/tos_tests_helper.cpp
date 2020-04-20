#include "tos_tests_helper.hpp"

#include <mln/core/algorithm/transform.hpp>
#include <mln/core/image/ndimage.hpp>
#include <mln/morpho/component_tree.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>

namespace
{

  template <class I, class J>
  void compare_tree_to_ref_T(const mln::morpho::component_tree<>& tree,        //
                             const J&                                           node_map,    //
                             const I&                                           parents_ref, //
                             const I&                                           roots_ref)
  {
    using P = mln::image_point_t<J>;

    const auto& par = tree.parent;
    auto pmax = node_map.domain().br();

    std::size_t    n = par.size();
    std::vector<P> repr_data(n + 1, pmax);

    P* repr = repr_data.data() + 1;
    mln_foreach_new(auto px, node_map.pixels())
    {
      int id = px.val();
      P   p    = px.point();
      repr[id] = std::min(repr[id], p);
    }
    repr[-1] = repr[0];

    // Build parent image
    auto roots = mln::transform(node_map, [&repr](int x) { return repr[x]; });
    auto parents = mln::transform(node_map, [&repr, &par](int x) { return repr[par[x]]; });

    ASSERT_IMAGES_EQ_EXP(roots, roots_ref);
    ASSERT_IMAGES_EQ_EXP(parents, parents_ref);
  }

}


void compare_tree_to_ref(const mln::morpho::component_tree<>&            tree,
                         const mln::image2d<int>&                        node_map,
                         const mln::image2d<mln::point2d>& parents,
                         const mln::image2d<mln::point2d>& roots)
{
  compare_tree_to_ref_T(tree, node_map, parents, roots);
}

void compare_tree_to_ref(const mln::morpho::component_tree<>&            tree,
                         const mln::image3d<int>&                        node_map,
                         const mln::image3d<mln::point3d>& parents,
                         const mln::image3d<mln::point3d>& roots)
{
  compare_tree_to_ref_T(tree, node_map, parents, roots);
}



