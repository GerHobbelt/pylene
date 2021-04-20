#pragma once

#include "mln/hierarchies/accumulators/hierarchy_accumulator_base.hpp"
#include "mln/hierarchies/graph.hpp"
#include "mln/hierarchies/hierarchy_tree.hpp"

#include <numeric>
#include <vector>

namespace mln
{
  template <typename I, typename O, Accumulator<I, O> AccumulatorType>
  inline std::vector<O> compute_attribute_from_accumulator(const HierarchyTree& tree, const AccumulatorType& acc,
                                                           std::vector<I> leaves_list    = std::vector<I>(),
                                                           std::vector<I> edges_list     = std::vector<I>(),
                                                           bool           exclude_leaves = false)
  {
    size_t tree_nb_vertices  = tree.get_nb_vertices();
    size_t graph_nb_vertices = tree.leaf_graph.get_nb_vertices();

    assert(leaves_list.size() == 0 || leaves_list.size() == graph_nb_vertices);
    assert(edges_list.size() == 0 || edges_list.size() == (tree_nb_vertices - graph_nb_vertices));

    std::vector<AccumulatorType> accumulators(tree_nb_vertices, acc);

    for (size_t i = 0; i < leaves_list.size(); ++i)
      accumulators[i].take(leaves_list[i]);

    for (size_t i = graph_nb_vertices; i < edges_list.size() + graph_nb_vertices; ++i)
      accumulators[i].take(edges_list[i - graph_nb_vertices]);

    size_t begin = exclude_leaves ? graph_nb_vertices : 0;

    for (size_t node = begin; node < tree_nb_vertices - 1; ++node)
    {
      int parent_node = tree.get_parent(node);
      if (parent_node == -1)
      {
        accumulators[node].invalidate();
        continue;
      }

      accumulators[parent_node].take(accumulators[node]);
    }

    std::vector<O> attribute(tree_nb_vertices);

    for (size_t i = 0; i < tree_nb_vertices; ++i)
      attribute[i] = accumulators[i].get_value();

    return attribute;
  }

  std::vector<int> depth_attribute(const HierarchyTree& tree);

  std::vector<int> area_attribute(const HierarchyTree& tree);

  std::vector<double> volume_attribute(const HierarchyTree& tree);

  /**
   * @return Vector of 0/1 for false/true and -1 for deleted nodes
   */
  std::vector<int> extrema_attribute(const HierarchyTree& tree);

  std::vector<double> height_attribute(const HierarchyTree& tree);

  std::vector<double> dynamic_attribute(const HierarchyTree& tree);
} // namespace mln