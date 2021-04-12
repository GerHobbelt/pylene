#pragma once

#include "mln/hierarchies/accumulators/hierarchy_accumulator_base.hpp"
#include "mln/hierarchies/graph.hpp"
#include "mln/hierarchies/hierarchy_tree.hpp"

#include <numeric>
#include <vector>

namespace mln
{
  struct HierarchyTraversal
  {
    enum Order
    {
      LEAVES_TO_ROOT = 0,
      ROOT_TO_LEAVES,
    } order = LEAVES_TO_ROOT;

    bool exclude_leaves = false;
  };

  inline std::vector<int> hierarchy_traversal(const HierarchyTree& tree, const HierarchyTraversal& traversal)
  {
    // Exclude root
    std::vector<int> res((traversal.exclude_leaves ? tree.get_nb_vertices() / 2 : tree.get_nb_vertices()) - 1);
    std::iota(res.begin(), res.end(), traversal.exclude_leaves ? tree.leaf_graph.get_nb_vertices() : 0);

    if (traversal.order == HierarchyTraversal::ROOT_TO_LEAVES)
      std::reverse(res.begin(), res.end());

    return res;
  }

  template <typename T, Accumulator<T> AccumulatorType>
  inline std::vector<T> compute_attribute_from_accumulator(const HierarchyTree& tree, const AccumulatorType& acc,
                                                           const HierarchyTraversal& traversal,
                                                           std::vector<T>            init_list = std::vector<T>())
  {
    int tree_nb_vertices = tree.get_nb_vertices();

    std::vector<AccumulatorType> accumulators(tree_nb_vertices, acc);
    for (int i = 0; i < tree_nb_vertices; ++i)
      accumulators[i].set_associated_node(i);

    for (size_t i = 0; i < init_list.size(); ++i)
      accumulators[i].init(init_list[i]);

    std::vector<int> traversal_vector = hierarchy_traversal(tree, traversal);

    if (traversal.order == HierarchyTraversal::LEAVES_TO_ROOT)
    {
      for (const auto& node : traversal_vector)
      {
        int parent_node = tree.get_parent(node);
        if (parent_node == -1)
        {
          accumulators[node].invalidate();
          continue;
        }

        accumulators[parent_node].merge(accumulators[node]);
      }
    }
    else if (traversal.order == HierarchyTraversal::ROOT_TO_LEAVES)
    {
      for (const auto& node : traversal_vector)
      {
        int parent_node = tree.get_parent(node);
        if (parent_node == -1)
        {
          accumulators[node].invalidate();
          continue;
        }

        accumulators[node].merge(accumulators[parent_node]);
      }
    }

    std::vector<T> attribute(tree_nb_vertices);

    for (int i = 0; i < tree_nb_vertices; ++i)
      attribute[i] = accumulators[i].get_value();

    return attribute;
  }

  std::vector<int> depth_attribute(const HierarchyTree& tree);

  std::vector<int> area_attribute(const HierarchyTree& tree);

  std::vector<int> volume_attribute(const HierarchyTree& tree);

  /**
   * @return Vector of 0/1 for false/true and -1 for deleted nodes
   */
  std::vector<int> extrema_attribute(const HierarchyTree& tree);

  std::vector<int> height_attribute(const HierarchyTree& tree);

  std::vector<int> dynamic_attribute(const HierarchyTree& tree);
} // namespace mln