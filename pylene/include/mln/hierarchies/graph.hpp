#pragma once

#include "mln/hierarchies/collections.hpp"

#include <algorithm>
#include <string>
#include <tuple>
#include <vector>

namespace mln
{
  using Edge = std::tuple<std::size_t, std::size_t, std::size_t>;

  // Image graph (4/8 adjacency)
  class Graph
  {
  public:
    Graph(std::size_t height, std::size_t width)
      : nb_vertices_(height * width)
      , height_(height)
      , width_(width)
      , qebt(QEBT(nb_vertices_))
    {
    }

    Graph(std::size_t nb_vertices, const std::string filename);

    inline std::size_t get_nb_vertices() const { return nb_vertices_; }

    inline std::size_t get_height() const { return height_; }

    inline std::size_t get_width() const { return width_; }

    inline const std::vector<Edge>& get_edges() const { return edges_; }

    void        add_edge(std::size_t source, std::size_t destination, std::size_t weight);
    std::size_t get_edge(std::size_t n) const;
    std::size_t weight_node(std::size_t n) const;

    void        add_edge(std::size_t source, std::size_t destination, std::size_t weight);
    std::size_t get_edge(std::size_t n);
    std::size_t weight_node(std::size_t n);

    void kruskal();


  private:
    std::size_t nb_vertices_;

    std::size_t height_;
    std::size_t width_;

    std::vector<std::tuple<std::size_t, std::size_t, std::size_t>> edges_;
    std::vector<std::tuple<std::size_t, std::size_t, std::size_t>> mst;
    QEBT                                                           qebt;
  };
} // namespace mln