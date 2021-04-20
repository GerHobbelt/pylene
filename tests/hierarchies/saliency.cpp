#include <mln/hierarchies/saliency.hpp>

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

TEST(Hierarchies, Saliency_Map)
{
  int gray_image[16] = {0,  0,  100, 0, //
                        0,  0,  75,  0, //
                        95, 80, 60,  0, //
                        0,  0,  0,   0};

  int expected_s_map_val[24][3] = {{0, 1, 0},   {14, 15, 0}, {13, 14, 0},  {12, 13, 0},  {11, 15, 0}, {7, 11, 0},
                                   {4, 5, 0},   {3, 7, 0},   {0, 4, 0},    {1, 5, 0},    {6, 10, 15}, {8, 9, 15},
                                   {9, 10, 20}, {2, 6, 25},  {10, 14, 60}, {10, 11, 60}, {5, 6, 75},  {6, 7, 60},
                                   {9, 13, 60}, {5, 9, 75},  {8, 12, 60},  {4, 8, 75},   {2, 3, 60},  {1, 2, 75}};

  Graph graph = create_graph_from_gray_image(gray_image, 4, 4);

  const HierarchyTree& bpt = graph.kruskal();

  auto s_map = saliency_map(bpt);

  for (int i = 0; i < 24; ++i)
  {
    auto expected_edge = expected_s_map_val[i];

    ASSERT_EQ(expected_edge[2], std::get<2>(s_map[i]));
  }
}

TEST(Hierarchies, Khalimsky_Grid)
{
  int gray_image[16] = {0,  0,  100, 0, //
                        0,  0,  75,  0, //
                        95, 80, 60,  0, //
                        0,  0,  0,   0};

  int expected_khalimsky_grid[9 * 9] = {0,  0,  0,  0,  75, 0,  60, 0, 0, //
                                        0,  0,  0,  0,  75, 0,  60, 0, 0, //
                                        0,  0,  0,  0,  75, 25, 60, 0, 0, //
                                        0,  0,  0,  0,  75, 0,  60, 0, 0, //
                                        75, 75, 75, 75, 75, 15, 60, 0, 0, //
                                        0,  0,  15, 0,  20, 0,  60, 0, 0, //
                                        60, 60, 60, 60, 60, 60, 60, 0, 0, //
                                        0,  0,  0,  0,  0,  0,  0,  0, 0, //
                                        0,  0,  0,  0,  0,  0,  0,  0, 0};

  Graph graph = create_graph_from_gray_image(gray_image, 4, 4);

  const HierarchyTree& bpt = graph.kruskal();

  image2d<double> khalimsky_grid = saliency_khalimsky_grid(bpt);

  for (int i = 0; i < 9 * 9; ++i)
  {
    ASSERT_EQ(expected_khalimsky_grid[i], static_cast<int>(khalimsky_grid({i % 9, i / 9})));
  }
}