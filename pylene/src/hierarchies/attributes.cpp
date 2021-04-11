#include "mln/hierarchies/attributes.hpp"
#include "mln/hierarchies/graph.hpp"

#include "mln/hierarchies/accumulators/area_accumulator.hpp"
#include "mln/hierarchies/accumulators/depth_accumulator.hpp"

namespace mln
{
  template <typename T, Accumulator<T> AccumulatorType>
  std::vector<T> compute_attribute_from_accumulator(const HierarchyTree& tree, TraversalOrder traversal_order)
  {
    int tree_nb_vertices = tree.get_nb_vertices();

    std::vector<AccumulatorType> accumulators(tree_nb_vertices);

    if (traversal_order == TraversalOrder::LEAVES_TO_ROOT)
    {
      for (int leaf = 0; leaf < tree.leaf_graph.get_nb_vertices(); ++leaf)
        accumulators[leaf].init();

      for (int node = 0; node < tree.get_nb_vertices() - 1; ++node)
      {
        int parent_node = tree.get_parent(node);
        if (parent_node == -1)
          continue;

        accumulators[parent_node].take(accumulators[node]);
      }
    }
    else if (traversal_order == TraversalOrder::ROOT_TO_LEAVES)
    {
      accumulators[tree_nb_vertices - 1].init();

      for (int node = tree.get_nb_vertices() - 2; node >= 0; --node)
      {
        int parent_node = tree.get_parent(node);
        if (parent_node == -1)
          continue;

        accumulators[node].take(accumulators[parent_node]);
      }
    }

    std::vector<T> attribute(tree_nb_vertices);

    for (int i = 0; i < tree_nb_vertices; ++i)
      attribute[i] = accumulators[i].extract_value();

    return attribute;
  }

  std::vector<int> depth_attribute(const HierarchyTree& tree)
  {
    return compute_attribute_from_accumulator<int, DepthAccumulator>(tree, TraversalOrder::ROOT_TO_LEAVES);
  }

  std::vector<int> area_attribute(const HierarchyTree& tree)
  {
    return compute_attribute_from_accumulator<int, AreaAccumulator>(tree, TraversalOrder::LEAVES_TO_ROOT);
  }

  std::vector<int> volume_attribute(const HierarchyTree& tree)
  {
    const Graph& leaf_graph = tree.leaf_graph;

    int tree_nb_vertices = tree.get_nb_vertices();

    std::vector<int> area = area_attribute(tree);

    std::vector<int> volume(tree_nb_vertices, 0);

    for (int i_node = leaf_graph.get_nb_vertices(); i_node < tree_nb_vertices - 1; ++i_node)
    {
      int parent_node = tree.get_parent(i_node);
      if (parent_node == -1)
      {
        volume[i_node] = -1;
        continue;
      }

      volume[i_node] += area[i_node] * abs(leaf_graph.weight_node(i_node) - leaf_graph.weight_node(parent_node));
      volume[parent_node] += volume[i_node];
    }

    return volume;
  }

  std::vector<int> extrema_attribute(const HierarchyTree& tree)
  {
    const Graph& leaf_graph = tree.leaf_graph;

    int tree_nb_vertices = tree.get_nb_vertices();

    std::vector<int> extrema(tree_nb_vertices, 1);
    std::fill_n(extrema.begin(), leaf_graph.get_nb_vertices(), 0);

    for (int i_node = leaf_graph.get_nb_vertices(); i_node < tree_nb_vertices - 1; ++i_node)
    {
      int parent_node = tree.get_parent(i_node);
      if (parent_node == -1)
      {
        extrema[i_node] = -1;
        continue;
      }

      bool same_weight = leaf_graph.weight_node(i_node) == leaf_graph.weight_node(parent_node);
      extrema[parent_node] &= same_weight && extrema[i_node];
      extrema[i_node] &= !same_weight;
    }

    return extrema;
  }

  std::vector<int> height_attribute(const HierarchyTree& tree)
  {
    const Graph& leaf_graph = tree.leaf_graph;

    int tree_nb_vertices = tree.get_nb_vertices();

    std::vector<int> deepest_altitude(tree_nb_vertices, std::numeric_limits<int>::max());

    int root = tree_nb_vertices - 1;

    for (int node = 0; node < root; ++node)
    {
      int parent_node = tree.get_parent(node);
      if (parent_node == -1)
      {
        deepest_altitude[node] = -1;
        continue;
      }

      if (node < leaf_graph.get_nb_vertices())
        deepest_altitude[node] = leaf_graph.weight_node(parent_node);

      deepest_altitude[parent_node] = std::min(deepest_altitude[parent_node], deepest_altitude[node]);
    }

    std::vector<int> height(tree_nb_vertices);
    for (int node = 0; node < root; ++node)
    {
      height[node] =
          deepest_altitude[node] == -1 ? -1 : leaf_graph.weight_node(tree.get_parent(node)) - deepest_altitude[node];
    }

    height[root] = leaf_graph.weight_node(root) - deepest_altitude[root];

    return height;
  }

  std::vector<int> dynamic_attribute(const HierarchyTree& tree)
  {
    const Graph& leaf_graph = tree.leaf_graph;

    int tree_nb_vertices = tree.get_nb_vertices();

    std::vector<int> deepest_altitude(tree_nb_vertices, std::numeric_limits<int>::max());
    std::vector<int> path_to_minima(tree_nb_vertices, -1);

    int root = tree_nb_vertices - 1;

    // Compute deepest altitude and path to deepest minima
    for (int node = leaf_graph.get_nb_vertices(); node < root; ++node)
    {
      int parent_node = tree.get_parent(node);
      if (parent_node == -1)
      {
        deepest_altitude[node] = -1;
        continue;
      }

      // Deepest non leaf node
      if (deepest_altitude[node] == std::numeric_limits<int>::max())
        deepest_altitude[node] = leaf_graph.weight_node(node);

      if (deepest_altitude[node] < deepest_altitude[parent_node])
      {
        deepest_altitude[parent_node] = deepest_altitude[node];
        path_to_minima[parent_node]   = node;
      }
    }

    std::vector<int> dynamic(tree_nb_vertices, -1);
    dynamic[root] = leaf_graph.weight_node(root) - deepest_altitude[root];

    std::vector<int> minima = extrema_attribute(tree);
    std::vector<int> nearest_minima(tree_nb_vertices, -1);

    for (int node = root - 1; node >= 0; --node)
    {
      int parent_node = tree.get_parent(node);
      if (parent_node == -1)
      {
        dynamic[node] = -1;
        continue;
      }

      if (node < leaf_graph.get_nb_vertices())
      {
        if (nearest_minima[parent_node] != -1)
          dynamic[node] = dynamic[nearest_minima[parent_node]];
        else
          dynamic[node] = 0;

        continue;
      }

      if (minima[node])
        nearest_minima[node] = node;
      else
        nearest_minima[node] = nearest_minima[parent_node];

      if (node == path_to_minima[parent_node])
        dynamic[node] = dynamic[parent_node];
      else
        dynamic[node] = leaf_graph.weight_node(parent_node) - deepest_altitude[node];
    }

    return dynamic;
  }

} // namespace mln