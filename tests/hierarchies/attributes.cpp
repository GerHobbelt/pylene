#include "mln/hierarchies/graph.hpp"
#include <mln/hierarchies/attributes.hpp>

#include <gtest/gtest.h>

using namespace mln;

static Graph* create_graph_from_gray_image(int* image, int height, int width)
{
  auto* res = new Graph(height, width);

  for (int j = 0; j < height; ++j)
  {
    for (int i = 0; i < width; ++i)
    {
      int array_pos = i + j * width;

      if (j != height - 1)
      {
        int weight = std::abs(image[array_pos] - image[array_pos + width]);
        res->add_edge(array_pos, array_pos + width, weight);
      }
      if (i != width - 1)
      {
        int weight = std::abs(image[array_pos] - image[array_pos + 1]);
        res->add_edge(array_pos, array_pos + 1, weight);
      }
    }
  }

  return res;
}

TEST(Hierarchies, Depth_Attribute)
{
  int gray_image[16] = {0,  0,  100, 0, //
                        0,  0,  75,  0, //
                        95, 80, 60,  0, //
                        0,  0,  0,   0};

  int expected_depth[31] = {3, 3, 3, 3, 3, 3, 5, 4, 5, 5, 5, 5, 6, 7, 8, 8,
                            2, 7, 6, 5, 4, 3, 2, 2, 1, 4, 4, 3, 2, 1, 0};

  Graph* graph = create_graph_from_gray_image(gray_image, 4, 4);

  HierarchyTree* bpt = graph->kruskal();

  int* depth = depth_attribute(*bpt);

  for (int i = 0; i < 31; ++i)
  {
    ASSERT_EQ(expected_depth[i], depth[i]);
  }

  delete[] depth;
  delete bpt;
  delete graph;
}

TEST(Hierarchies, Area_Attribute)
{
  int gray_image[16] = {0,  0,  100, 0, //
                        0,  0,  75,  0, //
                        95, 80, 60,  0, //
                        0,  0,  0,   0};

  int expected_area[31] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  1, 1,
                           2, 2, 3, 4, 5, 6, 2, 7, 4, 2, 2, 4, 5, 12, 16};

  Graph* graph = create_graph_from_gray_image(gray_image, 4, 4);

  HierarchyTree* bpt = graph->kruskal();

  int* area = area_attribute(*bpt);

  for (int i = 0; i < 31; ++i)
  {
    ASSERT_EQ(expected_area[i], area[i]);
  }

  delete[] area;
  delete bpt;
  delete graph;
}

TEST(Hierarchies, Volume_Attribute)
{
  int gray_image[16] = {0,  0,  100, 0, //
                        0,  0,  75,  0, //
                        95, 80, 60,  0, //
                        0,  0,  0,   0};

  int expected_volume[31] = {0, 0, 0, 0, 0, 0, 0, 0,   0,   0,  0,  0,  0,   0,   0,   0,
                             0, 0, 0, 0, 0, 0, 0, 420, 300, 10, 10, 40, 215, 815, 1115};

  Graph* graph = create_graph_from_gray_image(gray_image, 4, 4);

  HierarchyTree* bpt = graph->kruskal();

  int* volume = volume_attribute(*bpt);

  for (int i = 0; i < 31; ++i)
  {
    ASSERT_EQ(expected_volume[i], volume[i]);
  }

  delete[] volume;
  delete bpt;
  delete graph;
}

TEST(Hierarchies, Extrema_Attribute)
{
  int gray_image[16] = {0,  0,  100, 0, //
                        0,  0,  75,  0, //
                        95, 80, 60,  0, //
                        0,  0,  0,   0};

  int expected_extrema[31] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0};

  Graph* graph = create_graph_from_gray_image(gray_image, 4, 4);

  HierarchyTree* bpt = graph->kruskal();

  int* extrema = extrema_attribute(*bpt);

  for (int i = 0; i < 31; ++i)
  {
    ASSERT_EQ(expected_extrema[i], extrema[i]);
  }

  delete[] extrema;
  delete bpt;
  delete graph;
}