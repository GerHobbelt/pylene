#include "mln/hierarchies/saliency.hpp"
#include "mln/core/algorithm/fill.hpp"
#include "mln/hierarchies/attributes.hpp"

#include <algorithm>

namespace mln
{
  static std::vector<int> lca_preprocess(const HierarchyTree& tree) { return depth_attribute(tree); }

  // TODO Use sparse table to optimize LCA
  static int lca(const std::vector<int>& depth, const HierarchyTree& tree, int u, int v)
  {
    // Put u and v at the same level
    while (depth[u] != depth[v])
    {
      if (depth[u] > depth[v])
        u = tree.get_parent(u);
      else
        v = tree.get_parent(v);
    }

    // Go up until the ancestor is common
    while (u != v)
    {
      u = tree.get_parent(u);
      v = tree.get_parent(v);
    }

    return u;
  }

  std::vector<Edge> saliency_map(const HierarchyTree& tree)
  {
    std::vector<Edge> res;

    const Graph& leaf_graph = tree.leaf_graph;

    std::vector<int> depth = lca_preprocess(tree);

    for (Edge edge : leaf_graph.get_edges())
    {
      int merge_parent_node = lca(depth, tree, std::get<0>(edge), std::get<1>(edge));
      std::get<2>(edge)     = leaf_graph.weight_node(merge_parent_node);
      res.emplace_back(edge);
    }

    return res;
  }

  image2d<double> saliency_khalimsky_grid(const HierarchyTree& tree)
  {
    const Graph& leaf_graph = tree.leaf_graph;

    int height = leaf_graph.get_height();
    int width  = leaf_graph.get_width();

    int res_height = 2 * height + 1;
    int res_width  = 2 * width + 1;

    image2d<double> res(res_width, res_height);
    fill(res, 0);

    const std::vector<Edge>& s_map = saliency_map(tree);

    for (const auto& edge : s_map)
    {
      int    u = std::get<0>(edge);
      int    v = std::get<1>(edge);
      double w = std::get<2>(edge);

      int u_pos[2] = {u % width, u / width};
      int v_pos[2] = {v % width, v / width};

      int res_offset[2]   = {u_pos[0] - v_pos[0], u_pos[1] - v_pos[1]};
      int res_edge_pos[2] = {2 * v_pos[0] + 1 + res_offset[0], 2 * v_pos[1] + 1 + res_offset[1]};

      res({res_edge_pos[0], res_edge_pos[1]}) = w;
    }

    for (int y = 0; y < res_height; y += 2)
    {
      for (int x = 0; x < res_width; x += 2)
      {
        double max = std::numeric_limits<double>::min();

        if (y + 1 < height)
          max = std::max(max, res({x, y + 1}));
        if (x + 1 < width)
          max = std::max(max, res({x + 1, y}));
        if (y - 1 >= 0)
          max = std::max(max, res({x, y - 1}));
        if (x - 1 >= 0)
          max = std::max(max, res({x - 1, y}));

        res({x, y}) = max;
      }
    }

    return res;
  }

} // namespace mln