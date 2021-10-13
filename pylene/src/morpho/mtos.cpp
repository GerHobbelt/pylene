#include <mln/core/image/view/channel.hpp>
#include <mln/morpho/mtos.hpp>
#include <mln/morpho/private/satmaxtree.hpp>
#include <mln/morpho/private/trees_fusion.hpp>
#include <mln/morpho/tos.hpp>

namespace mln::morpho
{
  std::pair<component_tree<>, image2d<int>> mtos(image2d<rgb8> ima, point2d pstart)
  {
    mln::morpho::component_tree<> trees[3];
    mln::image2d<int>             nodemaps[3];
    std::vector<int>              depths[3];

    for (int c = 0; c < 3; c++)
    {
      std::tie(trees[c], nodemaps[c]) = mln::morpho::tos(mln::view::channel(ima, c), pstart);
      depths[c]                       = trees[c].compute_depth();
    }

    const auto [gos, tree_to_graph] = mln::morpho::details::compute_inclusion_graph(trees, nodemaps, depths, 3);
    auto depth_map                  = mln::morpho::details::compute_depth_map(gos, tree_to_graph, nodemaps);
    auto [t, nm]                    = mln::morpho::details::satmaxtree(depth_map, pstart);

    return {std::move(t), std::move(nm)};
  }
} // namespace mln::morpho