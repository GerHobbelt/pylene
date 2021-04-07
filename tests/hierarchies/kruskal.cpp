#include <mln/hierarchies/graph.hpp>

#include <gtest/gtest.h>
#include <fstream>

using namespace mln;

TEST(Hierarchies, Kruskal_easy)
{
  auto* res = new Graph(5, 5);

  res->add_edge(0, 1, 10);
  res->add_edge(0, 2, 6);
  res->add_edge(0, 3, 5);
  res->add_edge(1, 3, 15);
  res->add_edge(2, 3, 4);

  HierarchyTree* bpt = res->kruskal();

  ASSERT_EQ(res->mst.size(), 3);
  ASSERT_EQ(res->mst[0], std::make_tuple(2, 3, 4));
  ASSERT_EQ(res->mst[1], std::make_tuple(0, 3, 5));
  ASSERT_EQ(res->mst[2], std::make_tuple(0, 1, 10));
  
  delete bpt;
  delete res;
}

TEST(Hierarchies, Kruskal_med)
{
  auto* res = new Graph(10, 10);

  res->add_edge(0, 1, 2);
  res->add_edge(0, 4, 0);
  res->add_edge(1, 2, 2);
  res->add_edge(1, 5, 2);
  res->add_edge(2, 3, 0);
  res->add_edge(2, 6, 3);
  res->add_edge(3, 7, 1);
  res->add_edge(4, 5, 1);
  res->add_edge(5, 6, 2);
  res->add_edge(6, 7, 0);

  HierarchyTree* bpt = res->kruskal();

  ASSERT_EQ(res->mst.size(), 7);
  ASSERT_EQ(res->mst[0], std::make_tuple(0, 4, 0));
  ASSERT_EQ(res->mst[1], std::make_tuple(2, 3, 0));
  ASSERT_EQ(res->mst[2], std::make_tuple(6, 7, 0));
  ASSERT_EQ(res->mst[3], std::make_tuple(3, 7, 1));
  ASSERT_EQ(res->mst[4], std::make_tuple(4, 5, 1));
  ASSERT_EQ(res->mst[5], std::make_tuple(0, 1, 2));
  ASSERT_EQ(res->mst[6], std::make_tuple(1, 2, 2));

  delete bpt;
  delete res;
}