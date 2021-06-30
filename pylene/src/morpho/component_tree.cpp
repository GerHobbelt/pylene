#include <mln/morpho/component_tree.hpp>

#include <iostream>

namespace mln::morpho
{
  namespace internal
  {
    template <typename P, typename W>
    struct edge_t
    {
      P p;
      P q;
      W w;
    };
  } // namespace internal

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

  mln::image2d<double> component_tree<void>::saliency(mln::image2d<int> node_map, ::ranges::span<double> values) const
  {
    auto lca = [parent=parent, d=compute_depth()](int a, int b) {
      while (a != b)
      {
        if (d[a] < d[b])
          b = parent[b];
        else if (d[a] > d[b])
          a = parent[a];
        else
        {
          a = parent[a];
          b = parent[b];
        }
      }
      return b;
    };
    std::vector<internal::edge_t<point2d, int>> s_map;
    mln_foreach (auto p, node_map.domain())
    {
      if (p[0] == 440 && p[1] == 224)
        std::cout << "";
      for (auto q : c4.after(p))
      {
        if (node_map.domain().has(q))
          s_map.emplace_back(p, q, values[lca(node_map(p), node_map(q))]);
      }
    }

    const auto   kwidth  = node_map.width() * 2 - 1;
    const auto   kheight = node_map.height() * 2 - 1;
    image2d<double> res(kwidth, kheight);
    fill(res, 0);

    for (const auto [u, v, w] : s_map)
    {
      const auto dir = v - u;
      res(point2d{2 * u[0], 2 * u[1]} + dir) = w;
    }

    return res;
  }
} // namespace mln::morpho
