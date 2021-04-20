#include "mln/hierarchies/attributes.hpp"

#include "mln/hierarchies/accumulators/extrema_accumulator.hpp"
#include "mln/hierarchies/accumulators/height_accumulator.hpp"
#include "mln/hierarchies/accumulators/sum_accumulator.hpp"
#include "mln/hierarchies/accumulators/volume_accumulator.hpp"

namespace mln
{
  std::vector<int> depth_attribute(const HierarchyTree& tree)
  {
    int tree_nb_vertices = tree.get_nb_vertices();

    std::vector<int> depth(tree_nb_vertices, -1);

    // Set root depth to 0
    depth[tree_nb_vertices - 1] = 0;

    for (int node = tree_nb_vertices - 2; node >= 0; --node)
    {
      int parent_node = tree.get_parent(node);
      if (parent_node == -1)
        continue;

      depth[node] = depth[parent_node] + 1;
    }

    return depth;
  }

  std::vector<int> area_attribute(const HierarchyTree& tree)
  {
    std::vector<int> one_leaves(tree.leaf_graph.get_nb_vertices(), 1);
    return compute_attribute_from_accumulator<int, int>(tree, SumAccumulator<int>(), one_leaves);
  }

  std::vector<double> volume_attribute(const HierarchyTree& tree)
  {
    std::vector<std::tuple<double, double>> leaves(tree.leaf_graph.get_nb_vertices(), std::make_tuple<>(1, 0));

    auto diff_altitude = [&](int node) {
      int parent_node = tree.get_parent(node);
      if (parent_node == -1)
        return -1.;

      return tree.leaf_graph.weight_node(parent_node) - tree.leaf_graph.weight_node(node);
    };

    std::vector<std::tuple<double, double>> edges(tree.get_nb_vertices() - tree.leaf_graph.get_nb_vertices());
    for (int i_node = tree.leaf_graph.get_nb_vertices(); i_node < tree.get_nb_vertices() - 1; ++i_node)
      edges[i_node - tree.leaf_graph.get_nb_vertices()] = std::make_tuple<>(0, diff_altitude(i_node));

    // Root node
    edges[tree.get_nb_vertices() - tree.leaf_graph.get_nb_vertices() - 1] = std::make_tuple<>(0, 0);

    return compute_attribute_from_accumulator<std::tuple<double, double>, double>(tree, VolumeAccumulator(), leaves,
                                                                                  edges);
  }

  std::vector<int> extrema_attribute(const HierarchyTree& tree)
  {
    std::vector<std::tuple<bool, bool>> leaves(tree.leaf_graph.get_nb_vertices(), std::make_tuple<>(false, false));

    auto parent_same_altitude = [&](int node) {
      int parent_node = tree.get_parent(node);
      if (parent_node == -1)
        return false;

      return tree.leaf_graph.weight_node(parent_node) == tree.leaf_graph.weight_node(node);
    };

    std::vector<std::tuple<bool, bool>> edges(tree.get_nb_vertices() - tree.leaf_graph.get_nb_vertices());
    for (int i_node = tree.leaf_graph.get_nb_vertices(); i_node < tree.get_nb_vertices() - 1; ++i_node)
      edges[i_node - tree.leaf_graph.get_nb_vertices()] = std::make_tuple<>(true, parent_same_altitude(i_node));

    return compute_attribute_from_accumulator<std::tuple<bool, bool>, int>(tree, ExtremaAccumulator(), leaves, edges,
                                                                           true);
  }

  std::vector<double> height_attribute(const HierarchyTree& tree)
  {
    auto parent_altitude = [&](int node) {
      int parent_node = tree.get_parent(node);
      if (parent_node == -1)
        return -1.;

      return tree.leaf_graph.weight_node(parent_node);
    };

    std::vector<std::tuple<double, double>> leaves(tree.leaf_graph.get_nb_vertices());
    for (int leaf = 0; leaf < tree.leaf_graph.get_nb_vertices(); ++leaf)
      leaves[leaf] = std::make_tuple<>(parent_altitude(leaf), parent_altitude(leaf));

    std::vector<std::tuple<double, double>> edges(tree.get_nb_vertices() - tree.leaf_graph.get_nb_vertices());
    for (int i_node = tree.leaf_graph.get_nb_vertices(); i_node < tree.get_nb_vertices() - 1; ++i_node)
    {
      edges[i_node - tree.leaf_graph.get_nb_vertices()] =
          std::make_tuple<>(std::numeric_limits<int>::max(), parent_altitude(i_node));
    }

    int root = tree.get_nb_vertices() - 1;
    edges[root - tree.leaf_graph.get_nb_vertices()] =
        std::make_tuple<>(std::numeric_limits<int>::max(), tree.leaf_graph.weight_node(root));

    return compute_attribute_from_accumulator<std::tuple<double, double>, double>(tree, HeightAccumulator(), leaves,
                                                                                  edges);
  }

  std::vector<double> dynamic_attribute(const HierarchyTree& tree)
  {
    const Graph& leaf_graph = tree.leaf_graph;

    int tree_nb_vertices = tree.get_nb_vertices();

    std::vector<double> deepest_altitude(tree_nb_vertices, std::numeric_limits<double>::max());
    std::vector<double> path_to_minima(tree_nb_vertices, -1);

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
      if (deepest_altitude[node] == std::numeric_limits<double>::max())
        deepest_altitude[node] = leaf_graph.weight_node(node);

      if (deepest_altitude[node] < deepest_altitude[parent_node])
      {
        deepest_altitude[parent_node] = deepest_altitude[node];
        path_to_minima[parent_node]   = node;
      }
    }

    std::vector<double> dynamic(tree_nb_vertices, -1);
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