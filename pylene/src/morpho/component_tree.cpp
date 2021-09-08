#include <mln/morpho/component_tree.hpp>
#include <mln/morpho/lca.hpp>

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
    const auto lca = mln::morpho::lca(*this);

    const auto   kwidth  = node_map.width() * 2 - 1;
    const auto   kheight = node_map.height() * 2 - 1;
    image2d<double> res(kwidth, kheight);
    fill(res, 0);

    mln_foreach (auto p, node_map.domain())
    {
      for (auto q : c4.after(p))
      {
        if (node_map.domain().has(q))
        {
          const auto dir = q - p;
          res(point2d{2 * p[0], 2 * p[1]} + dir) = values[lca(node_map(p), node_map(q))];
        }
      }
    }
    mln_foreach(auto p, res.domain())
    {
      if (p[0] % 2 == 1 && p[1] % 2 == 1)
      {
        for (auto q : c4(p))
        {
          if (res.domain().has(q) && res(p) < res(q))
            res(p) = res(q);
        }
      }
    }

    return res;
  }
} // namespace mln::morpho
