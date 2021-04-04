#include <mln/hierarchies/segmentation.hpp>

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

TEST(Hierarchies, Segmentation_Watershed_graph)
{
  int gray_image[16] = {0,  0,  100, 0, //
                        0,  0,  75,  0, //
                        95, 80, 60,  0, //
                        0,  0,  0,   0};

  int expected_edge[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 5, 4};

  Graph* graph = create_graph_from_gray_image(gray_image, 4, 4);

  Graph*            watershed_graph = mln::watershed_graph(graph, WatershedAttribute::AREA);
  std::vector<Edge> watershed_edges = watershed_graph->get_edges();

  for (int i = 0; i < 15; ++i)
  {
    ASSERT_EQ(expected_edge[i], std::get<2>(watershed_edges[i]));
  }

  delete graph;
  delete watershed_graph;
}