#include <mln/morpho/lca.hpp>

#include <stack>

namespace mln::morpho
{
  namespace details
  {
    rmq_sparse_table::rmq_sparse_table(std::function<int(int)> data_fun, int n_data)
      : m_data(n_data * std::ceil(std::log2(n_data)), -1)
      , m_data_function(data_fun)
      , m_line_stride(n_data)
    {
      // Compute the RMQ for 1-length RMQ
      for (int i = 0; i < m_line_stride; i++)
        m_data[i] = i;

      // Compute dynamically the 2^n-length RMQ
      const int log2_size = std::ceil(std::log2(n_data));
      for (int i = 1; i < log2_size; i++)
      {
        for (int j = 0; j + std::pow(2, i) <= m_line_stride; j++)
        {
          const int oth_ind = (i - 1) * m_line_stride + (j + std::pow(2, i - 1));
          if (m_data_function(m_data[(i - 1) * m_line_stride + j]) <= m_data_function(m_data[oth_ind]))
            m_data[i * m_line_stride + j] = m_data[(i - 1) * m_line_stride + j];
          else
            m_data[i * m_line_stride + j] = m_data[oth_ind];
        }
      }
    }

    int rmq_sparse_table::operator()(int i, int j) const
    {
      assert(i <= j);
      const int k = i == j ? 0 : std::floor(std::log2(j - i));
      return m_data_function(m_data[k * m_line_stride + i]) < m_data_function(m_data[k * m_line_stride + (j - std::pow(2, k) + 1)])
                 ? m_data[k * m_line_stride + i]
                 : m_data[k * m_line_stride + (j - std::pow(2, k) + 1)];
    }

    bool rmq_sparse_table::has_been_processed() const { return m_data[0] >= 0; }

    restricted_rmq::restricted_rmq(const std::vector<int>& tab)
      : m_block_size(std::ceil(std::log2(tab.size()) / 2))
      , m_in_block_rmq(m_num_pos * m_table_line_stride, -1)
    {
      // View to the normalized table
      const auto normalized_tab = [&](int i) { return tab[i] - tab[(i / m_block_size) * m_block_size]; };

      // Computation of in-block RMQ
      int ind = 0;
      for (int i = 0; i < static_cast<int>(tab.size()); i++)
      {
        if (i != 0 && i % m_block_size == 0)
        {
          // If no ST has been built on this pattern, build it.
          if (m_in_block_rmq[ind * m_table_line_stride] < 0)
          {
          }
          ind = 0;
        }
        else
        {
          ind = ind << 1 | (normalized_tab(i) > 0 ? 1 : 0);
        }
      }

      // Computation of block RMQ
    }

    int restricted_rmq::operator()(const std::vector<int>& tab, int i, int j) const
    {
      assert(i <= j);
      (void)tab;
      return 0;
    }
  } // namespace details

  lca_t::lca_t(const component_tree<void>& t)
    : m_E(2 * t.parent.size() - 1, 0)
    , m_L(2 * t.parent.size() - 1, 0)
    , m_R(t.parent.size(), -1)
  {
    compute_euler_tour(t);
    auto fun = [&](int i) { return m_L[i]; };
    m_rmq    = details::rmq_sparse_table(fun, m_L.size());
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