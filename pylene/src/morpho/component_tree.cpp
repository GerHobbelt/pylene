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
}
