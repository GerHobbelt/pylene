#pragma once

#include "mln/core/colors.hpp"
#include "mln/hierarchies/collections.hpp"
#include <mln/core/image/ndimage.hpp>

#include <algorithm>
#include <cmath>
#include <string>
#include <tuple>
#include <vector>

namespace mln
{
  using Edge = std::tuple<int, int, int>;

  // Image graph (4/8 adjacency)
  class Graph
  {
  public:
    Graph(int height, int width)
      : nb_vertices_(height * width)
      , height_(height)
      , width_(width)
    {
    }

    Graph(int height, int width, const mln::image2d<rgb8>& image);

    inline int get_nb_vertices() const { return nb_vertices_; }

    inline int get_height() const { return height_; }

    inline int get_width() const { return width_; }

    inline const std::vector<Edge>& get_edges() const { return edges_; }

    void add_edge(int source, int destination, int weight);
    int  get_edge(int n) const;
    int  weight_node(int n) const;

    QEBT* kruskal();

    std::vector<Edge> mst;

  private:
    int nb_vertices_;

    int height_;
    int width_;

    std::vector<Edge> edges_;
  };
} // namespace mln