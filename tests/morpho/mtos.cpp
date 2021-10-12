#include <mln/morpho/mtos.hpp>

#include <mln/core/image/view/channel.hpp>
#include <mln/morpho/tos.hpp>

#include <mln/morpho/private/trees_fusion.hpp>

#include <gtest/gtest.h>

TEST(Morpho, GoS)
{
  mln::image2d<mln::rgb8> input = {
      {{255, 146, 0}, {31, 174, 0}, {45, 0, 255}, {255, 75, 0}, {31, 174, 0}}, //
      {{45, 0, 255}, {255, 75, 0}, {3, 0, 0}, {45, 0, 255}, {255, 255, 255}},  //
      {{3, 0, 0}, {31, 174, 0}, {255, 75, 0}, {31, 174, 0}, {255, 75, 0}},     //
      {{31, 174, 0}, {255, 255, 255}, {45, 0, 255}, {3, 0, 0}, {45, 0, 255}},  //
      {{255, 75, 0}, {31, 174, 0}, {255, 75, 0}, {31, 174, 0}, {255, 146, 0}}, //
  };

  std::vector<std::vector<int>> tree_to_graph_ref = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13},
                                                     {0, 2, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25},
                                                     {0, 26, 27, 28, 29, 30}};

  mln::morpho::component_tree<> trees[3];
  mln::image2d<int>             nodemaps[3];
  std::vector<int>              depths[3];

  for (int i = 0; i < 3; i++)
  {
    std::tie(trees[i], nodemaps[i]) = mln::morpho::tos(mln::view::channel(input, i), {0, 0});
    depths[i]                       = trees[i].compute_depth();
  }

  auto [graph, tree_to_graph] = mln::morpho::details::compute_inclusion_graph(trees, nodemaps, depths, 3);

  // Check number of nodes in the GoS
  ASSERT_EQ(graph.size(), 31);

  // Compare the map tree node -> graph node
  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < (int)tree_to_graph[i].size(); j++)
      ASSERT_EQ(tree_to_graph[i][j], tree_to_graph_ref[i][j]);
  }
}

TEST(Morpho, DepthMap)
{
  // Test depth map computation for GoS
}

TEST(Morpho, Satmaxtree)
{
  // Test satmaxtree computation from depth map
}