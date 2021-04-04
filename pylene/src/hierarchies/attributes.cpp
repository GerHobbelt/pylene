#include "mln/hierarchies/attributes.hpp"
#include "mln/hierarchies/graph.hpp"

#include <algorithm>

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
    int tree_nb_vertices = tree.get_nb_vertices();

    std::vector<int> area(tree_nb_vertices, 0);
    std::fill_n(area.begin(), tree.leaf_graph->get_nb_vertices(), 1);

    for (int node = 0; node < tree_nb_vertices - 1; ++node)
    {
      int parent_node = tree.get_parent(node);
      if (parent_node == -1)
      {
        area[node] = -1;
        continue;
      }

      area[parent_node] += area[node];
    }

    return area;
  }

  std::vector<int> volume_attribute(const HierarchyTree& tree)
  {
    const Graph* leaf_graph = tree.leaf_graph;

    int tree_nb_vertices = tree.get_nb_vertices();

    std::vector<int> area = area_attribute(tree);

    std::vector<int> volume(tree_nb_vertices, 0);

    for (int i_node = leaf_graph->get_nb_vertices(); i_node < tree_nb_vertices - 1; ++i_node)
    {
      int parent_node = tree.get_parent(i_node);
      if (parent_node == -1)
      {
        volume[i_node] = -1;
        continue;
      }

      volume[i_node] += area[i_node] * abs(leaf_graph->weight_node(i_node) - leaf_graph->weight_node(parent_node));
      volume[parent_node] += volume[i_node];
    }

    return volume;
  }

  std::vector<int> extrema_attribute(const HierarchyTree& tree)
  {
    const Graph* leaf_graph = tree.leaf_graph;

    int tree_nb_vertices = tree.get_nb_vertices();

    std::vector<int> extrema(tree_nb_vertices, 1);
    std::fill_n(extrema.begin(), leaf_graph->get_nb_vertices(), 0);

    for (int i_node = leaf_graph->get_nb_vertices(); i_node < tree_nb_vertices - 1; ++i_node)
    {
      int parent_node = tree.get_parent(i_node);
      if (parent_node == -1)
      {
        extrema[i_node] = -1;
        continue;
      }

      bool same_weight = leaf_graph->weight_node(i_node) == leaf_graph->weight_node(parent_node);
      extrema[parent_node] &= same_weight && extrema[i_node];
      extrema[i_node] &= !same_weight;
    }

    return extrema;
  }

  std::vector<int> height_attribute(const HierarchyTree& tree)
  {
    const Graph* leaf_graph = tree.leaf_graph;

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

      if (node < leaf_graph->get_nb_vertices())
        deepest_altitude[node] = leaf_graph->weight_node(parent_node);

      deepest_altitude[parent_node] = std::min(deepest_altitude[parent_node], deepest_altitude[node]);
    }

    std::vector<int> height(tree_nb_vertices);
    for (int node = 0; node < root; ++node)
    {
      height[node] =
          deepest_altitude[node] == -1 ? -1 : leaf_graph->weight_node(tree.get_parent(node)) - deepest_altitude[node];
    }

    height[root] = leaf_graph->weight_node(root) - deepest_altitude[root];

    return height;
  }

  std::vector<int> dynamic_attribute(const HierarchyTree& tree)
  {
    const Graph* leaf_graph = tree.leaf_graph;

    int tree_nb_vertices = tree.get_nb_vertices();

    std::vector<int> deepest_altitude(tree_nb_vertices, std::numeric_limits<int>::max());
    std::vector<int> path_to_minima(tree_nb_vertices, -1);

    int root = tree_nb_vertices - 1;

    // Compute deepest altitude and path to deepest minima
    for (int node = leaf_graph->get_nb_vertices(); node < root; ++node)
    {
      int parent_node = tree.get_parent(node);
      if (parent_node == -1)
      {
        deepest_altitude[node] = -1;
        continue;
      }

      // Deepest non leaf node
      if (deepest_altitude[node] == std::numeric_limits<int>::max())
        deepest_altitude[node] = leaf_graph->weight_node(node);

      if (deepest_altitude[node] < deepest_altitude[parent_node])
      {
        deepest_altitude[parent_node] = deepest_altitude[node];
        path_to_minima[parent_node]   = node;
      }
    }

    std::vector<int> dynamic(tree_nb_vertices, -1);
    dynamic[root] = leaf_graph->weight_node(root) - deepest_altitude[root];

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

      if (node < leaf_graph->get_nb_vertices())
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
        dynamic[node] = leaf_graph->weight_node(parent_node) - deepest_altitude[node];
    }

    return dynamic;
  }

} // namespace mln