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
