#include "mln/hierarchies/graph.hpp"

namespace mln
{
  void Graph::add_edge(unsigned source, unsigned destination, unsigned weight)
  {
    auto triplet = std::make_tuple(source, destination, weight);
    this->edges_.push_back(triplet);
  }

  unsigned Graph::get_edge(unsigned n) { return n - this->nb_vertices_; }

  unsigned Graph::weight_node(unsigned n)
  {
    auto triplet = this->mst[this->get_edge(n)];

    return std::get<2>(triplet);
  }


  static bool sortbyth(const std::tuple<unsigned, unsigned, unsigned>& a,
                       const std::tuple<unsigned, unsigned, unsigned>& b)
  {
    return (std::get<2>(a) < std::get<2>(b));
  }

  void Graph::kruskal()
  {
    for (unsigned xi = 0; xi < this->nb_vertices_; xi++)
      this->qebt.make_set(xi);

    std::sort(this->edges_.begin(), this->edges_.end(), sortbyth);

    for (unsigned i = 0; i < this->edges_.size(); i++)
    {
      auto edge = this->edges_[i];

      unsigned cx = this->qebt.find_canonical(std::get<0>(edge));
      unsigned cy = this->qebt.find_canonical(std::get<1>(edge));

      if (cx != cy)
        this->qebt.make_union(cx, cy);
      this->mst.push_back(edge);
    }
  }
} // namespace mln