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
    int*                      m_E = nullptr; ///< Euler tour
    int*                      m_D = nullptr; ///< Depth of each node in the Euler tour
    int*                      m_R = nullptr; ///< Representative of each node in the Euler tour (first pass on a node)
    details::rmq_sparse_table m_rmq;         ///< RMQ implementation in the LCA
  };
} // namespace mln::morpho