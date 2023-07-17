#include <mln/morpho/mtos.hpp>
#include <mln/morpho/private/satmaxtree.hpp>
#include <mln/morpho/private/trees_fusion.hpp>

#include <mln/core/image/view/channel.hpp>
#include <mln/morpho/tos.hpp>

#include <fixtures/ImageCompare/image_compare.hpp>
#include <gtest/gtest.h>

static std::vector<std::set<int>> graph_ref = { //
    {},                                         //
    {0},                                        //
    {0, 1},                                     //
    {0, 1, 14},                                 //
    {0, 1, 20},                                 //
    {0, 1, 24},                                 //
    {0, 1, 14},                                 //
    {0, 1, 14},                                 //
    {0, 7, 19},                                 //
    {0, 1, 22},                                 //
    {0, 14},                                    //
    {0, 10, 14},                                //
    {0, 11, 15},                                //
    {0, 16},                                    //
    {0},                                        //
    {0, 10, 14},                                //
    {0, 14},                                    //
    {0, 16, 29},                                //
    {0, 16},                                    //
    {0, 1, 14},                                 //
    {0, 1, 14},                                 //
    {0, 7, 14},                                 //
    {0, 1, 14},                                 //
    {0, 7, 22},                                 //
    {0, 1, 14},                                 //
    {6, 24, 28},                                //
    {0, 10, 15},                                //
    {0, 1, 20},                                 //
    {0, 1, 14},                                 //
    {0, 14},                                    //
    {0, 1, 19}};

static std::vector<std::vector<int>> tree_to_graph_ref = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13},
                                                          {0, 2, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25},
                                                          {0, 26, 27, 28, 29, 30}};

static mln::image2d<mln::rgb8> input = {
    {{255, 146, 0}, {31, 174, 0}, {45, 0, 255}, {255, 75, 0}, {31, 174, 0}}, //
    {{45, 0, 255}, {255, 75, 0}, {3, 0, 0}, {45, 0, 255}, {255, 255, 255}},  //
    {{3, 0, 0}, {31, 174, 0}, {255, 75, 0}, {31, 174, 0}, {255, 75, 0}},     //
    {{31, 174, 0}, {255, 255, 255}, {45, 0, 255}, {3, 0, 0}, {45, 0, 255}},  //
    {{255, 75, 0}, {31, 174, 0}, {255, 75, 0}, {31, 174, 0}, {255, 146, 0}}, //
};

static mln::image2d<std::uint16_t> depth_map_ref = {
    {0, 0, 2, 1, 3, 1, 2, 1, 3}, //
    {0, 0, 0, 0, 2, 1, 1, 1, 2}, //
    {4, 1, 1, 1, 3, 2, 2, 2, 3}, //
    {3, 1, 1, 1, 1, 1, 1, 1, 2}, //
    {4, 3, 3, 1, 1, 1, 3, 1, 2}, //
    {3, 1, 2, 1, 1, 1, 2, 1, 1}, //
    {3, 2, 3, 2, 2, 2, 3, 2, 3}, //
    {2, 2, 2, 1, 1, 1, 2, 1, 1}, //
    {3, 2, 3, 1, 1, 1, 3, 2, 3}, //
};

static std::vector<int> parent_ref = {-1, 0,  1,  1,  3,  3,  3,  3,  3,  3,  1,  10, 10,
                                      10, 10, 10, 10, 10, 10, 10, 10, 10, 21, 21, 10};

static mln::image2d<int> nodemap_ref = {
    {0, 0, 0, 0, 2, 1, 1, 1, 4, 3, 3, 3, 6, 3, 3, 3, 7},                  //
    {0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3},                  //
    {0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3},                  //
    {0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3},                  //
    {23, 1, 1, 1, 1, 1, 1, 1, 8, 3, 3, 3, 3, 3, 3, 3, 9},                 //
    {21, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3},                 //
    {21, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3},                 //
    {21, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3},                 //
    {22, 21, 21, 21, 21, 1, 1, 1, 1, 1, 1, 1, 11, 1, 1, 1, 3},            //
    {21, 10, 10, 10, 10, 1, 1, 1, 1, 1, 1, 1, 10, 1, 1, 1, 1},            //
    {21, 10, 10, 10, 10, 1, 1, 1, 1, 1, 1, 1, 10, 1, 1, 1, 1},            //
    {21, 10, 10, 10, 10, 1, 1, 1, 1, 1, 1, 1, 10, 1, 1, 1, 1},            //
    {21, 10, 10, 10, 19, 10, 10, 10, 10, 10, 10, 10, 12, 10, 10, 10, 14}, //
    {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10}, //
    {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10}, //
    {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10}, //
    {20, 10, 10, 10, 17, 10, 10, 10, 10, 10, 10, 10, 16, 10, 10, 10, 18}, //
};

TEST(Morpho, GoS)
{
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
  for (int i = 0; i < (int)graph.size(); i++)
  {
    ASSERT_EQ(graph[i].size(), graph_ref[i].size());
    auto ref_it = graph_ref[i].begin();
    for (auto it = graph[i].begin(); it != graph[i].end(); it++)
      ASSERT_EQ(*ref_it++, *it);
  }

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
  mln::image2d<int> nodemaps[3];

  for (int i = 0; i < 3; i++)
    std::tie(std::ignore, nodemaps[i]) = mln::morpho::tos(mln::view::channel(input, i), {0, 0});

  auto depth_map = mln::morpho::details::compute_depth_map(graph_ref, tree_to_graph_ref, nodemaps);

  ASSERT_IMAGES_EQ_EXP(depth_map, depth_map_ref);
}

TEST(Morpho, Satmaxtree)
{
  // Test satmaxtree computation from depth map
  auto [t, nm] = mln::morpho::details::satmaxtree(depth_map_ref, {0, 0});

  ASSERT_EQ(t.parent.size(), parent_ref.size());

  for (int i = 0; i < (int)parent_ref.size(); i++)
    ASSERT_EQ(t.parent[i], parent_ref[i]);

  ASSERT_IMAGES_EQ_EXP(nm, nodemap_ref);
}

TEST(Morpho, MToS)
{
  auto [t, nm] = mln::morpho::mtos(input, {0, 0});

  for (int i = 0; i < (int)parent_ref.size(); i++)
    ASSERT_EQ(t.parent[i], parent_ref[i]);

  ASSERT_IMAGES_EQ_EXP(nm, nodemap_ref);
}