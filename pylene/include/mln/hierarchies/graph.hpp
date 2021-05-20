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
  using Edge = std::tuple<int, int, double>;

  class Graph
  {
  public:
    Graph(int height, int width)
      : height_(height)
      , width_(width)
      , nb_vertices_(height_ * width_)
    {
    }

    /**
     * Generate a 4 adjacency graph from an image
     */
    explicit Graph(const mln::image2d<rgb8>& image);

    explicit Graph(const mln::image2d<uint8_t>& image);

    inline int get_nb_vertices() const { return nb_vertices_; }

    inline int get_height() const { return height_; }

    inline int get_width() const { return width_; }

    inline const std::vector<Edge>& get_edges() const { return edges_; }

    void   add_edge(int source, int destination, double weight);
    int    get_edge(int n) const;
    double weight_node(int n) const;

    QEBT kruskal();

    std::vector<Edge> mst;

  private:
    int height_;
    int width_;
    int nb_vertices_;

    std::vector<Edge> edges_;
  };
} // namespace mln