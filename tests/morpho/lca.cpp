#include <mln/morpho/lca.hpp>

#include <gtest/gtest.h>

TEST(Morpho, RMQ)
{
  std::vector<int>            tab = {1, 2, 5, 3, 4};
  std::vector<int>            expected{0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 3, 3, 3, 3, 4};
  mln::morpho::details::rmq_t rmq(tab);
  int                         exp_ind = 0;

  for (int i = 0; i < 5; i++)
  {
    for (int j = i; j < static_cast<int>(tab.size()); j++)
      ASSERT_EQ(rmq(i, j), expected[exp_ind++]);
  }
}

TEST(Morpho, LCA)
{
  mln::morpho::component_tree<void> t;
  t.parent = std::vector<int>{0, 0, 0, 1, 1, 1, 2, 2};
  std::vector<int>   expected{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 2, 0, 0, 0,
                            2, 2, 3, 1, 1, 0, 0, 4, 1, 0, 0, 5, 0, 0, 6, 2, 7};
  mln::morpho::lca_t lca(t);
  int                exp_ind = 0;

  for (int i = 0; i < 8; i++)
  {
    for (int j = i; j < 8; j++)
    {
      //std::cout << "lca(" << i << ", " << j << ") = " << lca(i, j) << "\n";
      ASSERT_EQ(lca(i, j), expected[exp_ind++]);
    }
  }
}