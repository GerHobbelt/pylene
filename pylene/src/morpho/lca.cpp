#include <mln/morpho/lca.hpp>

#include <cstring>
#include <stack>

namespace mln::morpho
{
  namespace details
  {
    rmq_sparse_table::rmq_sparse_table(int* __restrict tab, int n) noexcept { preprocess(tab, n); }

    rmq_sparse_table::~rmq_sparse_table() noexcept
    {
      std::free(m_sparse_table);
      std::free(m_pow2);
    }

    void rmq_sparse_table::preprocess(int* __restrict tab, int n) noexcept
    {
      mln_precondition(tab != nullptr);
      mln_precondition(n > 0);

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
      for (int i = 0; i < m_ncols; i++)
        m_sparse_table[i] = i;

      // Filling the other rows
      for (int i = 1; i < m_nrows; i++)
      {
        for (int j = 0; j < (m_ncols - m_pow2[i] + 1); j++)
        {
          m_sparse_table[i * m_ncols + j] = m_table[m_sparse_table[(i - 1) * m_ncols + j]] <=
                                                    m_table[m_sparse_table[(i - 1) * m_ncols + (j + m_pow2[i - 1])]]
                                                ? m_sparse_table[(i - 1) * m_ncols + j]
                                                : m_sparse_table[(i - 1) * m_ncols + (j + m_pow2[i - 1])];
        }
      }
    }

    int rmq_sparse_table::operator()(int a, int b) const noexcept
    {
      mln_precondition(a <= b);

      if (a == b)
        return a;

      const int k = std::floor(std::log2(b - a));
      return m_table[m_sparse_table[k * m_ncols + a]] < m_table[m_sparse_table[k * m_ncols + (b - m_pow2[k] + 1)]]
                 ? m_sparse_table[k * m_ncols + a]
                 : m_sparse_table[k * m_ncols + (b - m_pow2[k] + 1)];
    }
  } // namespace details

  lca::lca(const component_tree<void>& t) noexcept
  {
    const int num_node = t.parent.size();
    m_E                = (int*)std::malloc((2 * num_node - 1) * sizeof(int));
    m_D                = (int*)std::malloc((2 * num_node - 1) * sizeof(int));
    m_R                = (int*)std::malloc(num_node * sizeof(int));

    // First pass : computation of the children
    int*      children_info       = (int*)std::malloc(3 * num_node * sizeof(int));
    const int last_child_offset   = num_node;
    const int next_sibling_offset = 2 * num_node;

    std::memset(children_info, -1, num_node * sizeof(int));

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
    std::memset(m_R, -1, num_node * sizeof(int));
    int             i = 0;
    std::stack<int> st;
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

    m_rmq.preprocess(m_D, 2 * num_node - 1);
  }

  lca::~lca() noexcept
  {
    std::free(m_E);
    std::free(m_D);
    std::free(m_R);
  }

  int lca::operator()(int a, int b) const noexcept
  {
    int ar = m_R[a];
    int br = m_R[b];

    if (ar > br)
      std::swap(ar, br);

    return m_E[m_rmq(ar, br)];
  }
} // namespace mln::morpho