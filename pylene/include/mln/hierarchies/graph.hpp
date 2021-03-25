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
    Graph(unsigned nb_vertices)
      : nb_vertices_(nb_vertices)
      , qebt(QEBT(nb_vertices))
    {
    }

    Graph(unsigned nb_vertices, const std::string filename);

    void     add_edge(unsigned source, unsigned destination, unsigned weight);
    unsigned get_edge(unsigned n);
    unsigned weight_node(unsigned n);

    void kruskal();


  private:
    unsigned                                              nb_vertices_;
    std::vector<std::tuple<unsigned, unsigned, unsigned>> edges_;
    std::vector<std::tuple<unsigned, unsigned, unsigned>> mst;
    QEBT                                                  qebt;
  };


} // namespace mln