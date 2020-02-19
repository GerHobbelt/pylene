#include <mln/labeling/experimental/rag.hpp>


#include <mln/core/image/experimental/ndimage.hpp>
#include <mln/core/neighborhood/c4.hpp>
#include <mln/core/neighborhood/c8.hpp>


#include <fixtures/ImageCompare/image_compare.hpp>
#include <gtest/gtest.h>


using Graph = boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS>;


void check_edges(const Graph& graph, std::pair<int, int> edges_ref[])
{
  boost::graph_traits<Graph>::edge_iterator e, f;
  std::tie(e, f) = boost::edges(graph);
  for (int i = 0; e != f; ++e, ++i)
  {
    int x = static_cast<int>(boost::source(*e, graph));
    int y = static_cast<int>(boost::target(*e, graph));
    ASSERT_EQ(edges_ref[i], std::make_pair(x, y));
  }
}


TEST(Labeling, rag_test_1)
{


  mln::experimental::image2d<bool> ima = {{1, 0, 0, 0, 1}, //
                                          {0, 0, 0, 0, 0}, //
                                          {0, 0, 0, 0, 0}, //
                                          {0, 0, 0, 0, 0}, //
                                          {1, 0, 0, 0, 1}};

  mln::experimental::image2d<int16_t> IZ = {{1, 1, 0, 2, 2}, //
                                            {1, 1, 0, 2, 2}, //
                                            {0, 0, 0, 0, 0}, //
                                            {3, 3, 0, 4, 4}, //
                                            {3, 3, 0, 4, 4}};

  std::pair<int, int> edges_ref[] = {{1, 2}, {1, 3}, {2, 4}, {3, 4}};

  int       nlabel;
  Graph     graph;
  auto      lbl = mln::labeling::experimental::rag(ima, mln::experimental::c4, graph, nlabel);

  ASSERT_IMAGES_EQ_EXP(lbl, IZ);
  check_edges(graph, edges_ref);
}


TEST(Labeling, rag_test_2)
{


  mln::experimental::image2d<bool> ima = {{1, 0, 0, 0, 1}, //
                                          {0, 0, 0, 0, 0}, //
                                          {0, 0, 0, 0, 0}, //
                                          {0, 0, 0, 0, 0}, //
                                          {1, 0, 0, 0, 1}};

  mln::experimental::image2d<int16_t> IZ = {{1, 1, 0, 2, 2}, //
                                            {1, 1, 0, 2, 2}, //
                                            {0, 0, 0, 0, 0}, //
                                            {3, 3, 0, 4, 4}, //
                                            {3, 3, 0, 4, 4}};

  std::pair<int, int> edges_ref[] = {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}};


  int       nlabel;
  Graph     graph;
  auto      lbl = mln::labeling::experimental::rag(ima, mln::experimental::c8, graph, nlabel);

  ASSERT_IMAGES_EQ_EXP(lbl, IZ);
  check_edges(graph, edges_ref);
}




TEST(Labeling, rag_U)
{
  mln::experimental::image2d<bool> ima = {{0, 1, 0, 0, 0, 1, 0}, //
                                          {0, 1, 0, 1, 0, 1, 0}, //
                                          {0, 1, 0, 0, 0, 1, 0}, //
                                          {0, 1, 1, 1, 1, 1, 0}, //
                                          {0, 1, 1, 1, 1, 1, 0}, //
                                          {0, 0, 0, 0, 0, 0, 0}, //
                                          {0, 1, 0, 0, 0, 1, 0}, //
                                          {0, 1, 0, 0, 0, 1, 0}};


  mln::experimental::image2d<int16_t> ref = {{1, 1, 1, 0, 1, 1, 1}, //
                                             {1, 1, 0, 2, 0, 1, 1}, //
                                             {1, 1, 1, 0, 1, 1, 1}, //
                                             {1, 1, 1, 1, 1, 1, 1}, //
                                             {1, 1, 1, 1, 1, 1, 1}, //
                                             {0, 0, 1, 1, 1, 0, 0}, //
                                             {3, 3, 0, 1, 0, 4, 4}, //
                                             {3, 3, 3, 0, 4, 4, 4}};

  std::pair<int, int> edges_ref[] = {{1, 2}, {1, 3}, {1, 4}, {3, 4}};


  int       nlabel;
  Graph     graph;
  auto IZ = mln::labeling::experimental::rag(ima, mln::experimental::c4, graph, nlabel);
  ASSERT_EQ(nlabel, 4);
  ASSERT_IMAGES_EQ_EXP(ref, IZ);
  check_edges(graph, edges_ref);
}
