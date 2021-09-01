#include <mln/morpho/lca.hpp>

#include <cstring>
#include <stack>

namespace mln::morpho
{
  namespace details
  {
    rmq_sparse_table::rmq_sparse_table(int* tab, int n) { preprocess(tab, n); }

    rmq_sparse_table::~rmq_sparse_table() { std::free(m_sparse_table); }

    void rmq_sparse_table::preprocess(int* tab, int n)
    {
      mln_precondition(tab != nullptr);
      mln_precondition(n > 0);

      m_ncols        = n;
      m_nrows        = std::ceil(std::log2(m_ncols));
      m_sparse_table = (int*)std::malloc(m_nrows * m_ncols * sizeof(int));
      m_table        = tab;

      // First row : block of size 1
      for (int i = 0; i < m_ncols; i++)
        m_sparse_table[i] = i;

      // Filling the other rows
      for (int i = 1; i < m_nrows; i++)
      {
        for (int j = 0; j < (m_ncols - (int)std::pow(2, i)) + 1; j++)
        {
          m_sparse_table[i * m_ncols + j] =
              m_table[m_sparse_table[(i - 1) * m_ncols + j]] <=
                      m_table[m_sparse_table[(i - 1) * m_ncols + (j + (int)std::pow(2, i - 1))]]
                  ? m_sparse_table[(i - 1) * m_ncols + j]
                  : m_sparse_table[(i - 1) * m_ncols + (j + (int)std::pow(2, i - 1))];
        }
      }
    }

    int rmq_sparse_table::operator()(int a, int b) const
    {
      mln_precondition(a <= b);

      if (a == b)
        return a;

      const int k = std::floor(std::log2(b - a));
      return m_table[m_sparse_table[k * m_ncols + a]] <
                     m_table[m_sparse_table[k * m_ncols + (b - (int)std::pow(2, k) + 1)]]
                 ? m_sparse_table[k * m_ncols + a]
                 : m_sparse_table[k * m_ncols + (b - (int)std::pow(2, k) + 1)];
    }
  } // namespace details

  lca::lca(const component_tree<void>& t)
  {
    allocate(t.parent.size());
    compute_euler_tour(t, m_E, m_D, m_R);
    m_rmq.preprocess(m_D, 2 * t.parent.size() - 1);
  }

  void lca::allocate(std::size_t n)
  {
    m_E = (int*)std::malloc((2 * n - 1) * sizeof(int));
    m_D = (int*)std::malloc((2 * n - 1) * sizeof(int));
    m_R = (int*)std::malloc(n * sizeof(int));
  }

  void lca::deallocate()
  {
    std::free(m_E);
    std::free(m_D);
    std::free(m_R);
  }

  void lca::compute_euler_tour(const component_tree<void>& t, int* E, int* D, int* R)
  {
    int num_node = t.parent.size();

    // First pass : computation of the children
    int* first_child  = (int*)std::malloc(num_node * sizeof(int));
    int* last_child   = (int*)std::malloc(num_node * sizeof(int));
    int* next_sibling = (int*)std::malloc(num_node * sizeof(int));
    std::memset(first_child, -1, num_node * sizeof(int));

    for (int n = 1; n < num_node; n++)
    {
      const int p = t.parent[n];
      if (first_child[p] < 0)
      {
        first_child[p] = n;
        last_child[p]  = n;
      }
      next_sibling[last_child[p]] = n;
      last_child[p]               = n;
      next_sibling[n]             = -1;
    }

    // Second pass : computation of the Euler tour
    std::memset(R, -1, num_node * sizeof(int));
    int             i = 0;
    std::stack<int> st;
    st.push(0);
    while (!st.empty())
    {
      const int cur = st.top();
      st.pop();
      D[i] = t.parent[cur] == cur ? 0 : D[R[t.parent[cur]]] + 1;
      E[i] = cur;
      if (R[cur] < 0)
      {
        R[cur] = i;
        for (int c = first_child[cur]; c >= 0; c = next_sibling[c])
        {
          st.push(cur);
          st.push(c);
        }
      }
      i++;
    }

    std::free(first_child);
    std::free(last_child);
    std::free(next_sibling);
  }

  lca::~lca() { deallocate(); }

  int lca::operator()(int a, int b) const
  {
    int ar = m_R[a];
    int br = m_R[b];

    if (ar > br)
      std::swap(ar, br);

    return m_E[m_rmq(ar, br)];
  }
} // namespace mln::morpho