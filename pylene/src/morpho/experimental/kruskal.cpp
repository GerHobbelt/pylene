#include <mln/morpho/experimental/canvas/kruskal.hpp>

namespace mln::morpho::experimental::canvas
{
  kruskal_visitor_base::kruskal_visitor_base(int size)
    : parent(2 * size - 1)
    , m_root(size)
    , m_parent_c(size, -1)
    , m_rank(size, 0)
    , m_size(size)
  {
    // vector reserve
    for (int i = 0; i < m_size; i++)
    {
      m_root[i] = i;
      parent[i] = i;
    }
  }

  void kruskal_visitor_base::on_start() {}

  int kruskal_visitor_base::on_find(int e)
  {
    int r = e;
    while (m_parent_c[r] >= 0)
      r = m_parent_c[r];
    while (m_parent_c[e] >= 0)
    {
      auto tmp        = e;
      e               = m_parent_c[e];
      m_parent_c[tmp] = r;
    }
    return r;
  }

  void kruskal_visitor_base::on_union(int cx, int cy, mln::dontcare_t e)
  {
    (void)e;
    int tu     = m_root[cx];
    int tv     = m_root[cy];
    parent[tu] = m_size;
    parent[tv] = m_size;

    if (m_rank[cx] > m_rank[cy])
      std::swap(cx, cy);
    if (m_rank[cx] == m_rank[cy])
      m_rank[cy] += 1;
    m_parent_c[cx] = cy;
    int c          = cy;

    m_root[c] = m_size;
    make_set(m_size);
  }

  void kruskal_visitor_base::on_finish() {}

  void kruskal_visitor_base::make_set(int q)
  {
    parent[q] = -1;
    m_size += 1;
  }
} // namespace mln::morpho::experimental::canvas