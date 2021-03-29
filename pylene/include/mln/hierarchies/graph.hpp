#pragma once

#include "mln/hierarchies/collections.hpp"

#include <algorithm>
#include <string>
#include <tuple>
#include <vector>

namespace mln
{
  class Graph
  {
  public:
    Graph(std::size_t nb_vertices)
      : nb_vertices_(nb_vertices)
      , qebt(QEBT(nb_vertices))
    {
    }

    Graph(std::size_t nb_vertices, const std::string filename);

    void     add_edge(std::size_t source, std::size_t destination, std::size_t weight);
    std::size_t get_edge(std::size_t n);
    std::size_t weight_node(std::size_t n);

    void kruskal();


  private:
    std::size_t                                              nb_vertices_;
    std::vector<std::tuple<std::size_t, std::size_t, std::size_t>> edges_;
    std::vector<std::tuple<std::size_t, std::size_t, std::size_t>> mst;
    QEBT                                                  qebt;
  };


} // namespace mln