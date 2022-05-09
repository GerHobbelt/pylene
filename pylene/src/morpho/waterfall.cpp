#include <mln/morpho/canvas/unionfind.hpp>
#include <mln/morpho/waterfall.hpp>

#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighborhood/c4c8.hpp>
#include <mln/core/neighborhood/c8.hpp>

#include <set>

namespace mln::morpho
{
  image2d<int> waterfall_saliency(const component_tree<int>& t, const image2d<int>& nodemap, const mln::c4c8_t& nbh)
  {
    const auto        domain = nodemap.domain();
    mln::image2d<int> out    = mln::imconcretize(nodemap).set_init_value(0);
    auto              depth  = t.compute_depth();
    auto              lca    = [&depth, &t](int a, int b) {
      while (depth[a] > depth[b])
        a = t.parent[a];
      while (depth[b] > depth[a])
        b = t.parent[b];
      while (a != b)
      {
        a = t.parent[a];
        b = t.parent[b];
      }
      return a;
    };
    const int waterline = static_cast<int>(t.parent.size()) - 1;

    mln_foreach (auto p, domain)
    {
      if (nodemap(p) == waterline)
      {
        int vals[8];
        int count = 0;
        for (auto q : nbh(p))
        {
          if (domain.has(q) && nodemap(q) < waterline)
            vals[count++] = nodemap(q);
        }
        int max = 0;
        for (int i = 0; i < count - 1; i++)
        {
          for (int j = i + 1; j < count; j++)
          {
            if (vals[i] == vals[j])
              continue;
            max = std::max(max, t.values[lca(vals[i], vals[j])]);
          }
        }
        out(p) = max;
      }
    }

    return out;
  }
} // namespace mln::morpho