#include <mln/morpho/private/satmaxtree.hpp>

#include <mln/morpho/tos.hpp>

namespace mln::morpho::details
{
  std::pair<component_tree<>, image2d<int>> satmaxtree(image2d<std::uint16_t> depth_map)
  {
    auto [t, nm] = mln::morpho::tos(depth_map, {0, 0});

    const int                  num_node = t.parent.size();
    std::vector<std::uint16_t> delta(num_node, 0);
    for (int i = 1; i < num_node; i++)
    {
      delta[i] = delta[t.parent[i]];
      if (t.values[i] <= t.values[t.parent[i]])
        delta[i] += t.values[t.parent[i]] - t.values[i];
    }
    for (int i = 1; i < num_node; i++)
      t.values[i] += delta[i];

    std::vector<bool> pred(t.parent.size());
    for (int n = 1; n < (int)t.parent.size(); n++)
      pred[n] = t.values[n] > t.values[t.parent[n]] || t.parent[n] == 0;
    t.filter(CT_FILTER_DIRECT, nm, [&pred](int n) { return pred[n]; });

    return {std::move(t), std::move(nm)};
  }
} // namespace mln::morpho::details