#pragma once

#include "mln/hierarchies/accumulators/hierarchy_accumulator_base.hpp"
#include "mln/hierarchies/graph.hpp"
#include "mln/hierarchies/hierarchy_tree.hpp"

#include <numeric>
#include <vector>

namespace mln
{
  /**
   * Traverse a Hierarchy Tree using an accumulator to produce an attribute on it
   *
   * @tparam I Input type used for initialization of accumulators
   * @tparam O Output type that is the type of the returned attribute
   * @tparam AccumulatorType Accumulator type
   *
   * @param tree Hierarchy Tree that will be use to compute the attribute
   * @param acc Accumulator that define the attribute calculation
   * @param leaves_list Initialization list of the tree leaves for the accumulation
   * @param edges_list Initialization list of the tree internal nodes (graph edges) for the accumulation
   * @param exclude_leaves Exclude leaves from accumulation or not
   */
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

  /**
   * Compute the depth attribute without an accumulator
   *
   * @param tree Hierarchy Tree from which the depth attribute is computed
   *
   * @return Vector of depth attribute and -1 if the corresponding node is deleted
   */
  std::vector<int> depth_attribute(const HierarchyTree& tree);

  /**
   * Compute the area attribute using an accumulator
   *
   * @param tree Hierarchy Tree from which the area attribute is computed
   *
   * @return Vector of area attribute and -1 if the corresponding node is deleted
   */
  std::vector<int> area_attribute(const HierarchyTree& tree);

  /**
   * Compute the volume attribute using an accumulator
   *
   * @param tree Hierarchy Tree from which the volume attribute is computed
   *
   * @return Vector of volume attribute and -1 if the corresponding node is deleted
   */
  std::vector<double> volume_attribute(const HierarchyTree& tree);

  /**
   * Compute the extrema attribute using an accumulator
   *
   * @param tree Hierarchy Tree from which the extrema attribute is computed
   *
   * @return Vector of 0/1 for false/true (extrema attribute) and -1 if the corresponding node is deleted
   */
  std::vector<int> extrema_attribute(const HierarchyTree& tree);

  /**
   * Compute the height attribute using an accumulator
   *
   * @param tree Hierarchy Tree from which the height attribute is computed
   *
   * @return Vector of height attribute and -1 if the corresponding node is deleted
   */
  std::vector<double> height_attribute(const HierarchyTree& tree);

  /**
   * Compute the dynamic attribute without an accumulator
   *
   * @param tree Hierarchy Tree from which the dynamic attribute is computed
   *
   * @return Vector of dynamic attribute and -1 if the corresponding node is deleted
   */
  std::vector<double> dynamic_attribute(const HierarchyTree& tree);
} // namespace mln