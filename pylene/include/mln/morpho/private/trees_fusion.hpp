#pragma once

#include <mln/morpho/component_tree.hpp>

#include <set>
#include <tuple>
#include <vector>

namespace mln::morpho::details
{
  std::pair<std::vector<std::set<int>>, std::vector<std::vector<int>>>
  compute_inclusion_graph(component_tree<>* trees, image2d<int>* nodemaps, std::vector<int>* depths, int ntrees);

  image2d<std::uint16_t> compute_depth_map(const std::vector<std::set<int>>&    graph,
                                           const std::vector<std::vector<int>>& tree_to_graph, image2d<int>* nodemaps);
} // namespace mln::morpho::details