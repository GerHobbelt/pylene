#include <mln/morpho/lca.hpp>

#include <cstring>
#include <numeric>
#include <stack>

#include <iostream>

namespace mln::morpho
{
  namespace details
  {
    /*
     * RMQ Sparse Table implementation
     */
    rmq_sparse_table::rmq_sparse_table(int* __restrict tab, int n) noexcept { preprocess(tab, n); }

    rmq_sparse_table::~rmq_sparse_table() noexcept
    {
      if (m_sparse_table)
      {
        std::free(m_sparse_table);
        std::free(m_pow2);
      }
    }

    void rmq_sparse_table::preprocess(int* __restrict tab, int n) noexcept
    {
      mln_precondition(tab != nullptr);
      mln_precondition(n > 0);
      mln_precondition(!m_sparse_table);

      m_ncols        = n;
      m_nrows        = std::ceil(std::log2(m_ncols));
      m_sparse_table = (int*)std::malloc(m_nrows * m_ncols * sizeof(int));
      m_table        = tab;

      // Computation of the pow2 (faster than using std::pow)
      m_pow2    = (int*)std::malloc(m_nrows * sizeof(int));
      m_pow2[0] = 1;
      for (int i = 1; i < m_nrows; i++)
        m_pow2[i] = m_pow2[i - 1] * 2;

      // First row : block of size 1
      std::iota(m_sparse_table, m_sparse_table + m_ncols, 0);

      // Filling the other rows
      auto* prev = m_sparse_table;
      auto* curr = m_sparse_table + m_ncols;
      for (int i = 1; i < m_nrows; i++)
      {
        for (int j = 0; j < (m_ncols - m_pow2[i] + 1); j++)
          curr[j] = m_table[prev[j]] < m_table[prev[j + m_pow2[i - 1]]] ? prev[j] : prev[j + m_pow2[i - 1]];
        prev += m_ncols;
        curr += m_ncols;
      }
    }

    /*
     * Restricted RMQ implementation
     */
    restricted_rmq::restricted_rmq(int* tab, int n) noexcept { preprocess(tab, n); }


    restricted_rmq::~restricted_rmq() noexcept
    {
      if (m_in_block_tables)
      {
        std::free(m_in_block_tables);
        std::free(m_map_in_block_table);
        std::free(m_rmq_block_value);
        std::free(m_rmq_block_index);
      }
    }

    void restricted_rmq::preprocess(int* tab, int n) noexcept
    {
      // Setup data
      m_table      = tab;
      m_block_size = std::ceil(std::log2(n) / 2);
      m_num_block  = std::ceil(static_cast<double>(n) / m_block_size);

      // Compute the in-blocks range minimum query
      compute_in_block_tables(tab, n);

      m_rmq_blocks.preprocess(m_rmq_block_value, m_num_block);
    }

    int restricted_rmq::operator()(int a, int b) const noexcept
    {
      mln_precondition(a <= b);

      const int a_block            = a / m_block_size;
      const int b_block            = b / m_block_size;
      const int a_mod              = a % m_block_size;
      const int b_mod              = b % m_block_size;
      const int squared_block_size = m_block_size * m_block_size;

      // Case where the two indices are in the same block
      if (a_block == b_block)
        return a_block * m_block_size + m_in_block_tables[m_map_in_block_table[a_block] * m_block_size * m_block_size +
                                                          a_mod * m_block_size + b_mod];

      // Else
      int min = -1;

      // Computation of the left RMQ
      if (a_mod > 0)
        min = a_block * m_block_size +
              m_in_block_tables[m_map_in_block_table[a_block] * squared_block_size + (a_mod + 1) * m_block_size - 1];

      // Computation of the middle RMQ
      const int left  = a_block + (a_mod == 0 ? 0 : 1);
      const int right = b_block - (b_mod == m_block_size - 1 ? 0 : 1);
      if (left <= right)
      {
        const int mid_ind = m_rmq_block_index[m_rmq_blocks(left, right)];
        min               = min > -1 ? (m_table[mid_ind] < m_table[min] ? mid_ind : min) : mid_ind;
      }

      // Computation of the right RMQ
      if (b_mod < m_block_size - 1)
      {
        const int right_ind =
            b_block * m_block_size + m_in_block_tables[m_map_in_block_table[b_block] * squared_block_size + b_mod];
        min = min > -1 ? (m_table[right_ind] < m_table[min] ? right_ind : min) : right_ind;
      }
      return min;
    }

    void restricted_rmq::compute_in_block_tables(int* tab, int n) noexcept
    {
      const int squared_block_size = m_block_size * m_block_size;
      m_in_block_tables    = (int*)std::malloc(std::pow(2, m_block_size - 1) * squared_block_size * sizeof(int));
      m_map_in_block_table = (int*)std::malloc(m_num_block * sizeof(int));
      m_rmq_block_value    = (int*)std::malloc(m_num_block * sizeof(int));
      m_rmq_block_index    = (int*)std::malloc(m_num_block * sizeof(int));
      std::fill(m_in_block_tables, m_in_block_tables + static_cast<int>(std::pow(2, m_block_size - 1)) * squared_block_size, -1);

      for (int i = 0; i < n; i += m_block_size)
      {
        // Compute the index
        int ind = 0;
        int j;
        for (j = i + 1; j < i + m_block_size && j < n; j++)
          ind = (ind << 1) | (tab[j - 1] < tab[j] ? 1 : 0);
        while (j < i + m_block_size)
        {
          ind = (ind << 1) | 1;
          j++;
        }

        // Attribution of the index
        m_map_in_block_table[i / m_block_size] = ind;

        // Get the table associated to the index
        int* cur_tab = m_in_block_tables + ind * squared_block_size;

        // Compute the RMQ of the table if not done
        if (*cur_tab < 0)
        {
          // Function to get the table, even out of the table
          auto norm_tab = [tab, n, i](int j) {
            int ind = i + j;
            return ind >= n ? tab[n - 1] + ind - n + 1 : tab[ind];
          };

          // Filling the diagonale
          for (int c = 0; c < m_block_size; c++)
            cur_tab[c * m_block_size + c] = c;

          // Filling the other lines
          for (int l = 0; l < m_block_size; l++)
          {
            for (int c = l + 1; c < m_block_size; c++)
            {
              cur_tab[l * m_block_size + c] =
                  norm_tab(cur_tab[l * m_block_size + c - 1]) <= norm_tab(c) ? cur_tab[l * m_block_size + c - 1] : c;
            }
          }
        }

        m_rmq_block_value[i / m_block_size] = m_table[i + cur_tab[m_block_size - 1]];
        m_rmq_block_index[i / m_block_size] = i + cur_tab[m_block_size - 1];
      }
    }
  } // namespace details

  /*
   * Simple LCA implementation
   */
  simple_lca::simple_lca(const component_tree<void>& ct) noexcept
    : m_D(ct.compute_depth())
    , m_t(ct)
  {
  }

  int simple_lca::operator()(int a, int b) const noexcept
  {
    while (a != b)
    {
      if (m_D[a] < m_D[b])
        b = m_t.parent[b];
      else if (m_D[a] > m_D[b])
        a = m_t.parent[a];
      else
      {
        a = m_t.parent[a];
        b = m_t.parent[b];
      }
    }
    return b;
  }

  /*
   * LCA implementation
   */
  lca<void>::lca(const component_tree<void>& t) noexcept
  {
    mln_precondition(t.parent.size() > 0);

    const int num_node = static_cast<int>(t.parent.size());
    m_memory           = (int*)std::malloc((5 * num_node - 2) * sizeof(int));
    m_E                = m_memory;
    m_D                = m_memory + (2 * num_node - 1);
    m_R                = m_memory + (4 * num_node - 2);

    // First pass : computation of the children
    int*      children_info       = (int*)std::malloc(3 * num_node * sizeof(int));
    const int last_child_offset   = num_node;
    const int next_sibling_offset = 2 * num_node;

    std::fill(children_info, children_info + num_node, -1);

    for (int n = 1; n < num_node; n++)
    {
      const int p = t.parent[n];
      if (children_info[p] < 0)
      {
        children_info[p]                     = n;
        children_info[last_child_offset + p] = n;
      }
      children_info[next_sibling_offset + children_info[last_child_offset + p]] = n;
      children_info[last_child_offset + p]                                      = n;
      children_info[next_sibling_offset + n]                                    = -1;
    }

    // Second pass : computation of the Euler tour
    std::fill(m_R, m_R + num_node, -1);
    int                               i = 0;
    std::stack<int, std::vector<int>> st;
    st.push(0);
    while (!st.empty())
    {
      const int cur = st.top();
      st.pop();
      m_D[i] = t.parent[cur] == cur ? 0 : m_D[m_R[t.parent[cur]]] + 1;
      m_E[i] = cur;
      if (m_R[cur] < 0)
      {
        m_R[cur] = i;
        for (int c = children_info[cur]; c >= 0; c = children_info[next_sibling_offset + c])
        {
          st.push(cur);
          st.push(c);
        }
      }
      i++;
    }

    std::free(children_info);
  }

  lca<void>::~lca() noexcept { std::free(m_memory); }

  dyn_lca::dyn_lca() noexcept
    : m_kind(dyn_lca::kind::OTHER)
  {
  }

  dyn_lca::dyn_lca(dyn_lca::kind k, const component_tree<void>& t) noexcept { init(k, t); }

  dyn_lca::~dyn_lca()
  {
    if (m_kind == kind::SIMPLE)
      static_cast<simple_lca*>((void*)&m_lca)->~simple_lca();
    else if (m_kind == kind::LINEAR)
      static_cast<lca<>*>((void*)&m_lca)->~lca();
  }

  void dyn_lca::init(kind k, const component_tree<void>& t) noexcept
  {
    m_kind = k;
    if (k == kind::SIMPLE)
      new (&m_lca) simple_lca(t);
    else if (k == kind::LINEAR)
      new (&m_lca) lca<>(t);
  }

  int dyn_lca::operator()(int a, int b) const noexcept
  {
    mln_precondition(m_kind != kind::OTHER);

    if (m_kind == kind::SIMPLE)
      return static_cast<simple_lca*>((void*)&m_lca)->operator()(a, b);
    else
      return static_cast<lca<>*>((void*)&m_lca)->operator()(a, b);
  }
} // namespace mln::morpho