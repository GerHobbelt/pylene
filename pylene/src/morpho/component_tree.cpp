#include <mln/morpho/component_tree.hpp>


namespace mln::morpho
{
  void component_tree<void>::filter_direct(const std::vector<bool>& pred)
  {
    this->filter_direct_T([&pred](int x) { return pred[x]; });
  }


  std::vector<int> component_tree<void>::compute_depth() const
  {
    std::size_t n = parent.size();

    std::vector<int> depth(n);
    depth[0] = 0;

    for (std::size_t i = 1; i < n; ++i)
      depth[i] = depth[parent[i]] + 1;

    return depth;
  }

  std::vector<int> lca_preprocess(mln::morpho::component_tree<void>& tree) { return tree.compute_depth(); }

  // TODO Use sparse table to optimize LCA
  int lca(const std::vector<int>& depth, mln::morpho::component_tree<void>& tree, int u, int v)
  {
    // Put u and v at the same level
    while (depth[u] != depth[v])
    {
      if (depth[u] > depth[v])
        u = tree.parent[u];
      else
        v = tree.parent[v];
    }

    // Go until the ancestor is common
    while (u != v)
    {
      u = tree.parent[u];

      if (u == -1 && v == -1)
        return -1;

      v = tree.parent[v];
    }

    return u;
  }
} // namespace mln::morpho
