#pragma once

#include "mln/core/colors.hpp"
#include "mln/core/image/ndbuffer_image.hpp"
#include "mln/hierarchies/accumulators/max_accumulator.hpp"
#include "mln/hierarchies/accumulators/sum_accumulator.hpp"
#include "mln/hierarchies/canvas/hierarchical_segmentation_base.hpp"
#include "mln/hierarchies/graph.hpp"
#include "mln/morpho/component_tree.hpp"


namespace mln
{
  /**
   * Traverse a Component Tree using an accumulator to produce an attribute on it
   *
   * @tparam ArgumentType Input type used for initialization of accumulators
   * @tparam ResultType Output type that is the type of the returned attribute
   * @tparam AccumulatorType Accumulator type
   *
   * @param tree Component Tree that will be use to compute the attribute
   * @param acc Accumulator that define the attribute calculation
   * @param leaves_list Initialization list of the tree leaves for the accumulation
   * @param edges_list Initialization list of the tree internal nodes (graph edges) for the accumulation
   * @param exclude_leaves Exclude leaves from accumulation or not
   */
  template <typename ArgumentType, typename ResultType, Accumulator<ArgumentType, ResultType> AccumulatorType>
  inline std::vector<ResultType> compute_attribute_from_accumulator(
      mln::morpho::component_tree<uint8_t> tree, const Graph& leaf_graph, const AccumulatorType& acc,
      std::vector<ArgumentType> leaves_list = std::vector<ArgumentType>(),
      std::vector<ArgumentType> edges_list = std::vector<ArgumentType>(), bool exclude_leaves = false)
  {
    size_t tree_nb_vertices  = tree.parent.size() / 2;
    size_t graph_nb_vertices = leaf_graph.get_nb_vertices();

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
      int parent_node = tree.parent[node];
      if (parent_node == -1)
      {
        accumulators[node].invalidate();
        continue;
      }

      accumulators[parent_node].take(accumulators[node]);
    }

    std::vector<ResultType> attribute(tree_nb_vertices);

    for (size_t i = 0; i < tree_nb_vertices; ++i)
      attribute[i] = accumulators[i].get_value();

    return attribute;
  }

  /**
   * Get the attribute at the time of the merging
   *
   * @tparam AttributeType Attribute Type
   *
   * @param tree The Component Tree where we flood the attribute
   * @param attribute Attribute computed on the Component Tree
   */
  template <Attribute AttributeType>
  inline std::vector<AttributeType> get_computed_attribute(mln::morpho::component_tree<uint8_t> tree,
                                                           std::vector<AttributeType>           attribute,
                                                           const Graph&                         leaf_graph)
  {
    const Graph& leaf_graph = tree.leaf_graph;

    int tree_nb_vertices  = tree.parent.size() / 2;
    int graph_nb_vertices = leaf_graph.get_nb_vertices();
    int tree_root         = tree_nb_vertices - 1;

    std::vector<AttributeType> leaves(graph_nb_vertices, 0);

    std::vector<AttributeType> edges(tree_nb_vertices - graph_nb_vertices, std::numeric_limits<AttributeType>::min());
    for (int i_node = leaf_graph.get_nb_vertices(); i_node < tree_nb_vertices - 1; ++i_node)
    {
      if (leaf_graph.weight_node(tree.parent[i_node]) != leaf_graph.weight_node(i_node))
        edges[i_node - graph_nb_vertices] = attribute[i_node];
    }

    edges[tree_root - graph_nb_vertices] = attribute[tree_root];

    return compute_attribute_from_accumulator<AttributeType, AttributeType>(tree, MaxAccumulator<AttributeType>(),
                                                                            leaves, edges);
  }

  /**
   * Compute the watershed graph from an initial adjacency graph, the corresponding component tree and an attribute
   * To obtain the second hierarchy, the kruskal method have to be called on the watershed graph
   *
   * @tparam AttributeType Attribute Type
   *
   * @param graph Initial adjacency graph computed on an image
   * @param tree Component Tree computed on the graph with kruskal algorithm
   * @param attribute Attribute computed on the Component Tree
   */
  template <Attribute AttributeType>
  inline Graph watershed_graph(Graph& graph, mln::morpho::component_tree<uint8_t> tree,
                               std::vector<AttributeType> attribute, const Graph& leaf_graph)
  {
    int tree_nb_vertices = tree.parent.size() / 2;

    std::vector<AttributeType> tree_computed_attributes = get_computed_attribute<>(tree, attribute, leaf_graph);

    std::vector<AttributeType> min_tree_computed_attributes(tree_nb_vertices,
                                                            std::numeric_limits<AttributeType>::max());
    for (int node = 0; node < tree_nb_vertices - 1; ++node)
    {
      min_tree_computed_attributes[tree.parent[node]] =
          std::min(min_tree_computed_attributes[tree.parent[node]], tree_computed_attributes[node]);
    }

    Graph res(graph.get_height(), graph.get_width());

    const std::vector<Edge>& graph_mst = graph.mst;

    for (int i_node = graph.get_nb_vertices(); i_node < tree_nb_vertices; ++i_node)
    {
      const Edge& edge = graph_mst[graph.get_edge(i_node)];
      res.add_edge(std::get<0>(edge), std::get<1>(edge), min_tree_computed_attributes[i_node]);
    }

    return res;
  }
} // namespace mln