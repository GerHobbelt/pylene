#include <mln/hierarchies/graph.hpp>

#include <gtest/gtest.h>
#include <fstream>

using namespace mln;

TEST(Hierarchies, Kruskal)
{
  auto* res = new Graph(5, 5);

  res->add_edge(0, 1, 10);
  res->add_edge(0, 2, 6);
  res->add_edge(0, 3, 5);
  res->add_edge(1, 3, 15);
  res->add_edge(2, 3, 4);

  HierarchyTree* bpt = res->kruskal();

  std::ofstream myfile;
  myfile.open ("example.txt");

  for (std::size_t i = 0; i < res->mst.size(); i++)
  {
    auto tmp = res->mst[i];
    myfile << std::get<0>(tmp) << " " << std::get<1>(tmp) << " " << std::get<2>(tmp) << '\n';
  }

  myfile.close();

  delete bpt;
  delete res;
}