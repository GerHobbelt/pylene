#pragma once

#include <mln/morpho/component_tree.hpp>

#include <cmath>

namespace mln::morpho
{
  namespace details
  {
    /**
     * Implementation of the RMQ algorithm, preprocess the data in O(n log(n)) and execute a query in O(1)
     * The internal sparse table represents the table below :
     *
     *                  n
     *           +-------------+
     *  Log n {  |     ...     |
     *           +-------------+
     *
     */
    class rmq_sparse_table
    {
    public:
      rmq_sparse_table(const rmq_sparse_table&) = delete;
      rmq_sparse_table& operator=(const rmq_sparse_table&) = delete;

      rmq_sparse_table() = default;
      rmq_sparse_table(int* tab, int n) noexcept;
      ~rmq_sparse_table() noexcept;

      void preprocess(int* tab, int n) noexcept;

      int operator()(int a, int b) const noexcept;

    private:
      int* m_sparse_table = nullptr; ///< Sparse Table (ST)
      int* m_table;                  ///< Input table
      int  m_ncols;                  ///< Number of element of the input table
      int  m_nrows;                  ///< Number of rows in the ST (log(m_ncols))
      int* m_pow2;
    };

    inline int rmq_sparse_table::operator()(int a, int b) const noexcept
    {
      mln_precondition(a <= b);

      if (a == b)
        return a;

      const int k = std::floor(std::log2(b - a));
      return m_table[m_sparse_table[k * m_ncols + a]] < m_table[m_sparse_table[k * m_ncols + (b - m_pow2[k] + 1)]]
                 ? m_sparse_table[k * m_ncols + a]
                 : m_sparse_table[k * m_ncols + (b - m_pow2[k] + 1)];
    }


    /**
     * Implementation of the restricted RMQ, which has a preprocessing complexity in O(n) and a query complexity in O(1)
     * 
     * This RMQ is only applicable on tables which have adjacent elements differing by +/- 1
     */
    class restricted_rmq
    {
    public:
      restricted_rmq(const restricted_rmq&) = delete;
      restricted_rmq& operator=(const restricted_rmq&) = delete;

      restricted_rmq() = default;
      restricted_rmq(int* tab, int n) noexcept;

      ~restricted_rmq() noexcept;

      void preprocess(int* tab, int n) noexcept;

      int operator()(int a, int b) const noexcept;

    private:
      void compute_in_block_tables(int* tab, int n) noexcept;

    private:
      int*                      m_table = nullptr;              ///< Input table
      int                       m_block_size;                   ///< Size of a block (log(n) / 2)
      int                       m_num_block;                    ///< Number of block
      int*                      m_in_block_tables    = nullptr; ///< In-block RMQ
      int*                      m_map_in_block_table = nullptr; ///< Map from block index to in-block table index
      int*                      m_rmq_block_value    = nullptr; ///< Minimum value of each block in the table
      int*                      m_rmq_block_index    = nullptr; ///< Index for each block in which m_rmq_block_value occurs
      details::rmq_sparse_table m_rmq_blocks;                   ///< Sparse Table on m_rmq_block_value
    };
  } // namespace details

  class lca
  {
  public:
    lca()           = delete;
    lca(const lca&) = delete;
    lca& operator=(const lca&) = delete;

    // Constructor (preprocess the LCA from a tree)
    explicit lca(const component_tree<void>& t) noexcept;

    // Destructor (Deallocate the Euler tour tables)
    ~lca() noexcept;

    int operator()(int a, int b) const noexcept;

  private:
    int*                    m_E = nullptr; ///< Euler tour
    int*                    m_D = nullptr; ///< Depth of each node in the Euler tour
    int*                    m_R = nullptr; ///< Representative of each node in the Euler tour (first pass on a node)
    details::restricted_rmq m_rmq;         ///< RMQ implementation in the LCA
  };
} // namespace mln::morpho