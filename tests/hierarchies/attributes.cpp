#include "mln/hierarchies/graph.hpp"
#include <mln/hierarchies/attributes.hpp>

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

TEST(Hierarchies, Depth_Attribute)
{
  int gray_image[16] = {0,  0,  100, 0, //
                        0,  0,  75,  0, //
                        95, 80, 60,  0, //
                        0,  0,  0,   0};

  int expected_depth[31] = {3, 3, 3, 3, 3, 3, 5, 4, 5, 5, 5, 5, 6, 7, 8, 8,
                            2, 7, 6, 5, 4, 3, 2, 2, 1, 4, 4, 3, 2, 1, 0};

  Graph graph = create_graph_from_gray_image(gray_image, 4, 4);

  const HierarchyTree& bpt = graph.kruskal();

  std::vector<int> depth = depth_attribute(bpt);

  for (int i = 0; i < 31; ++i)
  {
    ASSERT_EQ(expected_depth[i], depth[i]);
  }
}

TEST(Hierarchies, Area_Attribute)
{
  int gray_image[16] = {0,  0,  100, 0, //
                        0,  0,  75,  0, //
                        95, 80, 60,  0, //
                        0,  0,  0,   0};

  int expected_area[31] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  1, 1,
                           2, 2, 3, 4, 5, 6, 2, 7, 4, 2, 2, 4, 5, 12, 16};

  Graph graph = create_graph_from_gray_image(gray_image, 4, 4);

  const HierarchyTree& bpt = graph.kruskal();

  std::vector<int> area = area_attribute(bpt);

  for (int i = 0; i < 31; ++i)
  {
    ASSERT_EQ(expected_area[i], area[i]);
  }
}

TEST(Hierarchies, Volume_Attribute)
{
  int gray_image[16] = {0,  0,  100, 0, //
                        0,  0,  75,  0, //
                        95, 80, 60,  0, //
                        0,  0,  0,   0};

  int expected_volume[31] = {0, 0, 0, 0, 0, 0, 0, 0,   0,   0,  0,  0,  0,   0,   0,   0,
                             0, 0, 0, 0, 0, 0, 0, 420, 300, 10, 10, 40, 215, 815, 1115};

  Graph graph = create_graph_from_gray_image(gray_image, 4, 4);

  const HierarchyTree& bpt = graph.kruskal();

  std::vector<double> volume = volume_attribute(bpt);

  for (int i = 0; i < 31; ++i)
  {
    ASSERT_EQ(expected_volume[i], volume[i]);
  }
}

TEST(Hierarchies, Extrema_Attribute)
{
  int gray_image[16] = {0,  0,  100, 0, //
                        0,  0,  75,  0, //
                        95, 80, 60,  0, //
                        0,  0,  0,   0};

  int expected_extrema[31] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0};

  Graph graph = create_graph_from_gray_image(gray_image, 4, 4);

  const HierarchyTree& bpt = graph.kruskal();

  std::vector<int> extrema = extrema_attribute(bpt);

  for (int i = 0; i < 31; ++i)
  {
    ASSERT_EQ(expected_extrema[i], extrema[i]);
  }
}

TEST(Hierarchies, Height_Attribute)
{
  int gray_image[16] = {0,  0,  100, 0, //
                        0,  0,  75,  0, //
                        95, 80, 60,  0, //
                        0,  0,  0,   0};

  int expected_height[31] = {0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0,  0,  0,  0, 0,
                             0, 0, 0, 0, 0, 0, 0, 60, 75, 5, 5, 10, 45, 75, 75};

  Graph graph = create_graph_from_gray_image(gray_image, 4, 4);

  const HierarchyTree& bpt = graph.kruskal();

  std::vector<double> height = height_attribute(bpt);

  for (int i = 0; i < 31; ++i)
  {
    ASSERT_EQ(expected_height[i], height[i]);
  }
}

TEST(Hierarchies, Dynamic_Attribute)
{
  int gray_image[16] = {0,  0,  100, 0, //
                        0,  0,  75,  0, //
                        95, 80, 60,  0, //
                        0,  0,  0,   0};

  int expected_dynamic[31] = {75, 75, 0,  75, 75, 75, 45, 75, 5,  5,  45, 75, 75, 75, 75, 75,
                              75, 75, 75, 75, 75, 75, 0,  75, 75, 45, 5,  45, 45, 75, 75};

  Graph graph = create_graph_from_gray_image(gray_image, 4, 4);

  const HierarchyTree& bpt = graph.kruskal();

  std::vector<double> dynamic = dynamic_attribute(bpt);

  for (int i = 0; i < 31; ++i)
  {
    ASSERT_EQ(expected_dynamic[i], dynamic[i]);
  }
}