#include <mln/hierarchies/segmentation.hpp>

#include "mln/hierarchies/attributes.hpp"

#include <gtest/gtest.h>

using namespace mln;

static Graph create_graph_from_gray_image(int* image, int height, int width)
{
  Graph res(height, width);

  for (int j = 0; j < height; ++j)
  {
    for (int i = 0; i < width; ++i)
    {
      int array_pos = i + j * width;

      if (j != height - 1)
      {
        int weight = std::abs(image[array_pos] - image[array_pos + width]);
        res.add_edge(array_pos, array_pos + width, weight);
      }
      if (i != width - 1)
      {
        int weight = std::abs(image[array_pos] - image[array_pos + 1]);
        res.add_edge(array_pos, array_pos + 1, weight);
      }
    }
  }

  return res;
}

TEST(Hierarchies, Segmentation_Watershed_graph)
{
  int gray_image[16] = {0,  0,  100, 0, //
                        0,  0,  75,  0, //
                        95, 80, 60,  0, //
                        0,  0,  0,   0};

  int expected_edge[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 5, 4};

  Graph graph = create_graph_from_gray_image(gray_image, 4, 4);

  Graph             watershed_graph = mln::watershed_graph(graph, mln::area_attribute);
  std::vector<Edge> watershed_edges = watershed_graph.get_edges();

  for (int i = 0; i < 15; ++i)
  {
    ASSERT_EQ(expected_edge[i], std::get<2>(watershed_edges[i]));
  }
}

TEST(Hierarchies, Segmentation_Threshold_cut_labelization)
{
  int gray_image[16] = {0,  0,  100, 0, //
                        0,  0,  75,  0, //
                        95, 80, 60,  0, //
                        0,  0,  0,   0};

  double threshold = 0.5;

  int expected_labels[31] = {24, 24, 28, 23, 24, 24, 28, 23, 28, 28, 28, 23, 23, 23, 23, 23,
                             24, 23, 23, 23, 23, 23, 24, 23, 24, 28, 28, 28, 28, 29, -1};

  Graph graph = create_graph_from_gray_image(gray_image, 4, 4);

  const HierarchyTree& bpt = graph.kruskal();

  std::vector<int> labels = threshold_cut_labelization(bpt, threshold);

  for (int i = 0; i < 31; ++i)
  {
    ASSERT_EQ(expected_labels[i], labels[i]);
  }
}

TEST(Hierarchies, Segmentation_Mean_color_per_node)
{
  int gray_image[16] = {0,  0,  100, 0, //
                        0,  0,  75,  0, //
                        95, 80, 60,  0, //
                        0,  0,  0,   0};

  image2d<rgb8> rgb_image = {{{0, 1, 2}, {2, 3, 4}, {105, 106, 107}, {108, 109, 110}},
                             {{11, 12, 13}, {14, 15, 16}, {92, 93, 94}, {20, 21, 22}},
                             {{117, 118, 119}, {106, 107, 108}, {89, 90, 91}, {32, 33, 34}},
                             {{35, 36, 37}, {38, 39, 40}, {41, 42, 43}, {44, 45, 46}}};

  rgb8 expected_mean_color[31] = {
      {0, 1, 2},    {2, 3, 4},    {105, 106, 107}, {108, 109, 110}, {11, 12, 13},    {14, 15, 16},
      {92, 93, 94}, {20, 21, 22}, {117, 118, 119}, {106, 107, 108}, {89, 90, 91},    {32, 33, 34},
      {35, 36, 37}, {38, 39, 40}, {41, 42, 43},    {44, 45, 46},    {1, 2, 3},       {42, 43, 44},
      {41, 42, 43}, {39, 40, 41}, {38, 39, 40},    {35, 36, 37},    {12, 13, 14},    {45, 46, 47},
      {6, 7, 8},    {90, 91, 92}, {111, 112, 113}, {101, 102, 103}, {101, 102, 103}, {68, 69, 70},
      {53, 54, 55},
  };

  Graph graph = create_graph_from_gray_image(gray_image, 4, 4);

  const HierarchyTree& bpt = graph.kruskal();

  std::vector<rgb8> mean_color = mean_color_per_node(bpt, rgb_image);

  for (int i = 0; i < 31; ++i)
  {
    ASSERT_EQ(expected_mean_color[i], mean_color[i]);
  }
}