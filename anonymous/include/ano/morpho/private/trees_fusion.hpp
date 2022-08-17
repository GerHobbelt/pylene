#pragma once

#include <ano/morpho/component_tree.hpp>

#include <set>
#include <tuple>
#include <vector>

namespace ano::morpho::details
{
  /// \brief Compute the inclusion graph of the trees given in input
  /// \param trees The input trees
  /// \param nodemaps The nodemaps related to each tree
  /// \param depths The depths attribute of each tree
  /// \param The number of tree given in argument
  /// \return A pair (graph, tree_to_graph) where graph is the inclusion graph and tree_to_graph
  ///         is a map (tree_index, tree_node) -> graph_node
  std::pair<std::vector<std::set<int>>, std::vector<std::vector<int>>>
  compute_inclusion_graph(component_tree<>* trees, image2d<int>* nodemaps, std::vector<int>* depths, int ntrees);

  /// \brief Compute the depth map of an inclusion graph
  /// \param graph The inclusion graph
  /// \param tree_to_graph A map (tree_index, tree_node) -> graph_node
  /// \param nodemaps The nodemaps of each tree which built the inclusion graph
  /// \return The depth map of the inclusion graph (The depth should not exceed 2^16)
  /// FIXME: For future optimization, the arguments should be change to
  /// (const std::vector<std::set<int>>&    graph, const std::vector<int>& tree_to_graph, image2d<int>* nodemaps, const
  /// std::vector<int>& tree_shapes)
  image2d<std::uint16_t> compute_depth_map(const std::vector<std::set<int>>&    graph,
                                           const std::vector<std::vector<int>>& tree_to_graph, image2d<int>* nodemaps);
} // namespace ano::morpho::details