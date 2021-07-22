#include <mln/morpho/lca.hpp>

#include <stack>

namespace mln::morpho
{
  namespace details
  {
    rmq_t::rmq_t(const std::vector<int>& tab)
      : m_tab(tab.size() * tab.size(), 0)
      , m_stride(tab.size())
    {
      const int s = static_cast<int>(tab.size());

      // Fill diagonale
      for (int i = 0; i < s; i++)
        m_tab[i * m_stride + i] = i;

      // Fill the table
      for (int i = 1; i < s; i++)
      {
        int j = i + 1;
        while (j < s)
        {
          if (tab[m_tab[i * m_stride + (j - 1)]] < tab[j])
            m_tab[i * m_stride + j] = m_tab[i * m_stride + (j - 1)];
          else
            m_tab[i * m_stride + j] = j;
          j++;
        }
      }
    }

    int rmq_t::operator()(int i, int j) const
    {
      assert(i <= j);
      return m_tab[i * m_stride + j];
    }
  } // namespace details

  lca_t::lca_t(const component_tree<void>& t)
    : m_E(2 * t.parent.size() - 1, 0)
    , m_L(2 * t.parent.size() - 1, 0)
    , m_R(t.parent.size(), -1)
  {
    compute_euler_tour(t);
    m_rmq = details::rmq_t(m_L);
  }

  void lca_t::compute_euler_tour(const component_tree<void>& t)
  {
    // 1. Compute children
    std::vector<std::vector<int>> children(t.parent.size());
    for (int n = 1; n < static_cast<int>(t.parent.size()); n++)
      children[t.parent[n]].push_back(n);

    // 2. DFS
    int             cur_id = 1;
    std::stack<int> st;

    // Initalize for root
    m_R[0] = 0;
    for (auto it = children[0].rbegin(); it != children[0].rend(); it++)
    {
      st.push(0);
      st.push(*it);
    }

    while (!st.empty())
    {
      // Pop the first element
      const auto cur = st.top();
      st.pop();

      // If I never visited the node,  set the representative and push the children
      if (m_R[cur] < 0)
      {
        m_R[cur] = cur_id;
        for (auto it = children[cur].rbegin(); it != children[cur].rend(); it++)
        {
          st.push(cur);
          st.push(*it);
        }
      }
      m_E[cur_id] = cur;
      m_L[cur_id] = cur > 0 ? m_L[m_R[t.parent[cur]]] + 1 : 0;

      cur_id++;
    }
  }

  int lca_t::operator()(int i, int j) const
  {
    auto a = m_R[i];
    auto b = m_R[j];

    if (a > b)
      std::swap(a, b);
    return m_E[m_rmq(a, b)];
  }
} // namespace mln::morpho