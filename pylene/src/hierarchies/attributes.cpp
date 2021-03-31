#include "mln/hierarchies/attributes.hpp"
#include "mln/hierarchies/graph.hpp"

#include <algorithm>

namespace mln
{
  int* depth_attribute(const HierarchyTree& tree)
  {
    int tree_nb_vertices = tree.get_nb_vertices();

    int* depth = new int[tree_nb_vertices];
    std::fill_n(depth, tree_nb_vertices, -1);

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

  int* area_attribute(const HierarchyTree& tree)
  {
    int tree_nb_vertices = tree.get_nb_vertices();

    int* area = new int[tree_nb_vertices];
    std::fill_n(area, tree_nb_vertices, 0);

    std::fill_n(area, tree.leaf_graph->get_nb_vertices(), 1);

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

  int* volume_attribute(const HierarchyTree& tree)
  {
    const Graph* leaf_graph = tree.leaf_graph;

    int tree_nb_vertices = tree.get_nb_vertices();

    int* area = area_attribute(tree);

    int* volume = new int[tree_nb_vertices];
    std::fill_n(volume, tree_nb_vertices, 0);

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

    delete[] area;
    return volume;
  }

  int* extrema_attribute(const HierarchyTree& tree)
  {
    const Graph* leaf_graph = tree.leaf_graph;

    int tree_nb_vertices = tree.get_nb_vertices();

    int* extrema = new int[tree_nb_vertices];
    std::fill_n(extrema, tree_nb_vertices, 1);

    std::fill_n(extrema, leaf_graph->get_nb_vertices(), 0);

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

} // namespace mln