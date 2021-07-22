#pragma once

#include <mln/morpho/component_tree.hpp>

namespace mln::morpho
{
  namespace details
  {
    class rmq_t
    {
    public:
      rmq_t() = default;
      explicit rmq_t(const std::vector<int>& tab);

      int operator()(int i, int j) const;

    private:
        std::vector<int> m_tab;
        std::size_t m_stride;
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
    std::vector<int> m_E;
    std::vector<int> m_L;
    std::vector<int> m_R;
    details::rmq_t     m_rmq;
  };
} // namespace mln::morpho