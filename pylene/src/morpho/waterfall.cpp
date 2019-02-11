#include <mln/morpho/waterfall.hpp>

namespace mln::morpho::details
{
  void mst_waterfall_visitor::on_init(int n)
  {
    m_nlbl = n;
    m_diameter.resize(n, 0);
    m_roots.resize(n);
    std::iota(m_roots.begin(), m_roots.end(), 0);
  }

  void mst_waterfall_visitor::on_union(int p, int rp, int q, int rq, mln::dontcare_t)
  {
    int min, max; // Using tie instead of SB due to lifetime error
    std::tie(min, max) = std::minmax(m_diameter[m_roots[rp]], m_diameter[m_roots[rq]]);
    m_diameter.push_back(std::max(min + 1, max));
    mst.emplace_back(p, q, min + 1);
    m_roots[rp] = m_nlbl;
    m_roots[rq] = m_nlbl;
    m_nlbl++;
  }

  void waterfall_bpt_visitor::on_init(int n)
  {
    m_nlbl = n;
    value.resize(n, 0);
    parent.resize(n);
    m_roots.resize(n);
    std::iota(m_roots.begin(), m_roots.end(), 0);
  }

  void waterfall_bpt_visitor::on_union(mln::dontcare_t, int rp, mln::dontcare_t, int rq, int w)
  {
    parent.push_back(m_nlbl);
    parent[m_roots[rp]] = m_nlbl;
    parent[m_roots[rq]] = m_nlbl;

    m_roots[rp] = m_nlbl;
    m_roots[rq] = m_nlbl;

    value.push_back(w);
    m_nlbl++;
  }
} // namespace mln::morpho::details