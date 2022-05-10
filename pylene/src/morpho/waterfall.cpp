#include <mln/morpho/canvas/unionfind.hpp>
#include <mln/morpho/waterfall.hpp>

#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighborhood/c4c8.hpp>
#include <mln/core/neighborhood/c8.hpp>

#include <set>

namespace mln::morpho
{
  image2d<int> waterfall_saliency(const component_tree<int>& t, const image2d<int>& nodemap, const mln::c4c8_t& /*nbh*/)
  {
    const auto        domain   = nodemap.domain();
    mln::image2d<int> out      = mln::imconcretize(nodemap).set_init_value(0);
    const auto        is_wline = [&t, &nodemap](const auto& p) { return t.values[nodemap(p)] > 0; };
    mln_foreach (auto p, domain)
    {
      if (is_wline(p))
        out(p) = t.values[nodemap(p)];
    }

    return out;
  }
} // namespace mln::morpho