#include <mln/morpho/private/lca.hpp>

#include <gtest/gtest.h>

TEST(Morpho, RMQ_Sparse_Table)
{
  // Setup data
  int       test[]     = {1, 2, 5, 4, 7, 0};
  const int expected[] = {0, 0, 0, 0, 0, 5, 1, 1, 1, 1, 5, 2, 3, 3, 5, 3, 3, 5, 4, 5, 5};
  const int size       = 6;

  // Setup RMQ
  auto rmq = mln::morpho::details::rmq_sparse_table(test, size);

  // Test
  int exp_ind = 0;
  for (int i = 0; i < size; i++)
  {
    for (int j = i; j < size; j++)
      ASSERT_EQ(rmq(i, j), expected[exp_ind++]);
  }
}

TEST(Morpho, RMQ_Restricted)
{
  // Setup data
  int       test[]     = {3, 4, 5, 4, 3, 2, 3, 4, 5};
  const int expected[] = {0, 0, 0, 0, 0, 5, 5, 5, 5, 1, 1, 1, 4, 5, 5, 5, 5, 2, 3, 4, 5, 5, 5,
                          5, 3, 4, 5, 5, 5, 5, 4, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 7, 7, 8};
  const int size       = 9;

  // Setup RMQ
  auto rmq = mln::morpho::details::restricted_rmq(test, size);

  // Test
  int exp_ind = 0;
  for (int i = 0; i < size; i++)
  {
    for (int j = i; j < size; j++)
      ASSERT_EQ(rmq(i, j), expected[exp_ind++]);
  }
}

TEST(Morpho, SimpleLCA)
{
  // Setup data
  mln::morpho::component_tree<void> t;
  t.parent             = std::vector<int>{0, 0, 0, 1, 1, 1, 2, 2};
  const int expected[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 2, 0, 0,
                          0, 2, 2, 3, 1, 1, 0, 0, 4, 1, 0, 0, 5, 0, 0, 6, 2, 7};

  // Setup LCA
  mln::morpho::simple_lca lca(t);

  // Test
  int exp_ind = 0;
  for (int i = 0; i < 8; i++)
  {
    for (int j = i; j < 8; j++)
      ASSERT_EQ(lca(i, j), expected[exp_ind++]);
  }
}

TEST(Morpho, SparseTableLCA)
{
  // Setup data
  mln::morpho::component_tree<void> t;
  t.parent             = std::vector<int>{0, 0, 0, 1, 1, 1, 2, 2};
  const int expected[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 2, 0, 0,
                          0, 2, 2, 3, 1, 1, 0, 0, 4, 1, 0, 0, 5, 0, 0, 6, 2, 7};

  // Setup LCA
  mln::morpho::lca<mln::morpho::details::rmq_sparse_table> lca(t);

  // Test
  int exp_ind = 0;
  for (int i = 0; i < 8; i++)
  {
    for (int j = i; j < 8; j++)
      ASSERT_EQ(lca(i, j), expected[exp_ind++]);
  }
}

TEST(Morpho, LinearLCA)
{
  // Setup data
  mln::morpho::component_tree<void> t;
  t.parent             = std::vector<int>{0, 0, 0, 1, 1, 1, 2, 2};
  const int expected[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 2, 0, 0,
                          0, 2, 2, 3, 1, 1, 0, 0, 4, 1, 0, 0, 5, 0, 0, 6, 2, 7};

  // Setup LCA
  mln::morpho::lca<> lca(t);

  // Test
  int exp_ind = 0;
  for (int i = 0; i < 8; i++)
  {
    for (int j = i; j < 8; j++)
      ASSERT_EQ(lca(i, j), expected[exp_ind++]);
  }
}

TEST(Morpho, DynSimpleLCA)
{
  // Setup data
  mln::morpho::component_tree<void> t;
  t.parent             = std::vector<int>{0, 0, 0, 1, 1, 1, 2, 2};
  const int expected[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 2, 0, 0,
                          0, 2, 2, 3, 1, 1, 0, 0, 4, 1, 0, 0, 5, 0, 0, 6, 2, 7};

  // Setup LCA
  mln::morpho::dyn_lca lca(mln::morpho::dyn_lca::kind::SIMPLE, t);

  // Test
  int exp_ind = 0;
  for (int i = 0; i < 8; i++)
  {
    for (int j = i; j < 8; j++)
      ASSERT_EQ(lca(i, j), expected[exp_ind++]);
  }
}

TEST(Morpho, DynLinearLCA)
{
  // Setup data
  mln::morpho::component_tree<void> t;
  t.parent             = std::vector<int>{0, 0, 0, 1, 1, 1, 2, 2};
  const int expected[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 2, 0, 0,
                          0, 2, 2, 3, 1, 1, 0, 0, 4, 1, 0, 0, 5, 0, 0, 6, 2, 7};

  // Setup LCA
  mln::morpho::dyn_lca lca(mln::morpho::dyn_lca::kind::LINEAR, t);

  // Test
  int exp_ind = 0;
  for (int i = 0; i < 8; i++)
  {
    for (int j = i; j < 8; j++)
      ASSERT_EQ(lca(i, j), expected[exp_ind++]);
  }
}