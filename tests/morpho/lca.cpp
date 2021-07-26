#include <mln/morpho/lca.hpp>

#include <gtest/gtest.h>

TEST(Morpho, RMQ_Sparse_Table)
{
  // Setup data
  std::vector<int> test     = {1, 2, 5, 4, 7, 0};
  std::vector<int> expected = {0, 0, 0, 0, 0, 5, 1, 1, 1, 1, 5, 2, 3, 3, 5, 3, 3, 5, 4, 5, 5};

  // Setup RMQ
  auto f   = [&](int i) { return test[i]; };
  auto rmq = mln::morpho::details::rmq_sparse_table(f, test.size());

  // Test
  int exp_ind = 0;
  const int size = test.size();
  for (int i = 0; i < size; i++)
  {
    for (int j = i; j < size; j++)
      ASSERT_EQ(rmq(i, j), expected[exp_ind++]);
  }
}

TEST(Morpho, restricted_RMQ) {}

TEST(Morpho, LCA)
{
  // Setup data
  mln::morpho::component_tree<void> t;
  t.parent = std::vector<int>{0, 0, 0, 1, 1, 1, 2, 2};
  std::vector<int> expected{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 2, 0, 0,
                            0, 2, 2, 3, 1, 1, 0, 0, 4, 1, 0, 0, 5, 0, 0, 6, 2, 7};

  // Setup LCA
  mln::morpho::lca_t lca(t);

  // Test
  int exp_ind = 0;
  for (int i = 0; i < 8; i++)
  {
    for (int j = i; j < 8; j++)
      ASSERT_EQ(lca(i, j), expected[exp_ind++]);
  }
}