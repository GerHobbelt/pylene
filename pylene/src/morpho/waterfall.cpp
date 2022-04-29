#include <mln/morpho/canvas/unionfind.hpp>
#include <mln/morpho/waterfall.hpp>

#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighborhood/c8.hpp>

#include <iostream>
#include <set>

namespace mln::morpho
{
  namespace details
  {
    void waterfall_visitor::init(int nlbl) noexcept
    {
      // Result initialization
      m_nlbl = nlbl + 1;
      values.resize(m_nlbl, 0);
      std::iota(parent.begin(), parent.end(), 0);
      parent.resize(m_nlbl);

      // Helpers
      m_save = m_nlbl;
      m_zpar.resize(m_nlbl);
      std::iota(m_zpar.begin(), m_zpar.end(), 0);
      m_diameter.resize(m_nlbl, 0);
    }

    void waterfall_visitor::visit(int p, int q) noexcept
    {
      using mln::morpho::canvas::impl::zfindroot;

      int rp = zfindroot(m_zpar.data(), p);
      int rq = zfindroot(m_zpar.data(), q);
      if (rp != rq)
      {
        m_zpar.push_back(m_nlbl);
        m_zpar[rp]      = m_nlbl;
        m_zpar[rq]      = m_nlbl;
        auto [min, max] = std::minmax(m_diameter[rp], m_diameter[rq]);
        m_diameter.push_back(std::max(min + 1, max));
        m_mst.emplace_back(p, q, min + 1);
        m_nlbl++;
      }
    }

    void waterfall_visitor::finalize() noexcept
    {
      using mln::morpho::canvas::impl::zfindroot;

      m_nlbl = m_save;
      std::ranges::sort(m_mst, [](const auto& a, const auto& b) { return std::get<2>(a) < std::get<2>(b); });
      m_zpar.resize(m_nlbl);
      std::iota(m_zpar.begin(), m_zpar.end(), 0);
      for (auto [p, q, w] : m_mst)
      {
        int rp = zfindroot(m_zpar.data(), p);
        int rq = zfindroot(m_zpar.data(), q);
        parent.push_back(m_nlbl);
        parent[rp] = m_nlbl;
        parent[rq] = m_nlbl;
        m_zpar.push_back(m_nlbl);
        m_zpar[rp] = m_nlbl;
        m_zpar[rq] = m_nlbl;
        values.push_back(w);
        m_nlbl++;
      }
    }
  } // namespace details

  image2d<int> waterfall_saliency(const component_tree<int>& t, const image2d<int>& nodemap)
  {
    const auto        domain = nodemap.domain();
    mln::image2d<int> out    = mln::imconcretize(nodemap).set_init_value(0);
    auto              depth  = t.compute_depth();
    /*auto depth = [&t]() {
      auto res = std::vector(t.parent.size(), 0);
      for (int i = t.parent.size() - 2; i > 0; i--)
        res[i] = res[t.parent[i]] + 1;
      return res;
    }();*/
    auto lca = [&depth, &t](int a, int b) {
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

    const int waterline = t.parent.size() - 1;
    mln_foreach (auto p, domain)
    {
      if (nodemap(p) == waterline)
      {
        int           tab[8];
        std::set<int> track;
        for (auto q : mln::c8(p))
        {
          if (domain.has(q) && nodemap(q) < waterline && !track.contains(nodemap(q)))
          {
            tab[track.size()] = nodemap(q);
            track.insert(nodemap(q));
          }
        }
        if (track.size() == 2)
          out(p) = t.values[lca(tab[0], tab[1])];
      }
    }

    mln_foreach (auto p, domain)
    {
      if (nodemap(p) == waterline)
      {
        int value[4];
        int nval = 0;
        for (auto q : mln::c4(p))
          if (domain.has(q) && nodemap(q) == waterline)
            value[nval++] = out(q);
        if (nval > 2)
          out(p) = *std::max_element(value, value + nval);
      }
    }

    return out;
  }
} // namespace mln::morpho