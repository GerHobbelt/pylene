#pragma once

#include <mln/morpho/component_tree.hpp>

#include <cmath>
#include <functional>

namespace mln::morpho
{
  namespace details
  {
    class rmq_sparse_table
    {
    public:
      rmq_sparse_table() = default;
      rmq_sparse_table(std::function<int(int)> data_fun, int n_data);

      int  operator()(int i, int j) const;
      bool has_been_processed() const;

    private:
      std::vector<int>        m_data;
      std::function<int(int)> m_data_function;
      int                     m_line_stride;
    };

    class restricted_rmq
    {
    public:
      restricted_rmq() = default;
      explicit restricted_rmq(const std::vector<int>& tab);

      int operator()(const std::vector<int>& tab, int i, int j) const;

    private:
      int              m_block_size;
      int              m_table_line_stride = m_block_size * m_block_size;
      int              m_num_pos           = std::pow(2, m_block_size - 1);
      std::vector<int> m_in_block_rmq;
    };
  } // namespace details

  class lca_t
  {
  public:
    explicit lca_t(const component_tree<void>& t);

    int operator()(int a, int b) const;

  private:
    void compute_euler_tour(const component_tree<void>& t);

  private:
    std::vector<int>          m_E;
    std::vector<int>          m_L;
    std::vector<int>          m_R;
    details::rmq_sparse_table m_rmq;
  };
} // namespace mln::morpho