#include <ano/core/image/view/channel.hpp>
#include <ano/morpho/mtos.hpp>
#include <ano/morpho/private/satmaxtree.hpp>
#include <ano/morpho/private/trees_fusion.hpp>
#include <ano/morpho/tos.hpp>

namespace ano::morpho
{
  std::pair<component_tree<>, image2d<int>> mtos(image2d<rgb8> ima, point2d pstart)
  {
    ano::morpho::component_tree<> trees[3];
    ano::image2d<int>             nodemaps[3];
    std::vector<int>              depths[3];

    for (int c = 0; c < 3; c++)
    {
      std::tie(trees[c], nodemaps[c]) = ano::morpho::tos(ano::view::channel(ima, c), pstart);
      depths[c]                       = trees[c].compute_depth();
    }

    const auto [gos, tree_to_graph] = ano::morpho::details::compute_inclusion_graph(trees, nodemaps, depths, 3);
    auto depth_map                  = ano::morpho::details::compute_depth_map(gos, tree_to_graph, nodemaps);
    auto [t, nm]                    = ano::morpho::details::satmaxtree(depth_map, pstart);

    return {std::move(t), std::move(nm)};
  }
} // namespace ano::morpho