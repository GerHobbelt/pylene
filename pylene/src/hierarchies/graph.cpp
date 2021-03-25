#include "mln/hierarchies/graph.hpp"

namespace mln
{
  void Graph::add_edge(std::size_t source, std::size_t destination, std::size_t weight)
  {
    auto triplet = std::make_tuple(source, destination, weight);
    this->edges_.push_back(triplet);
  }

  std::size_t Graph::get_edge(std::size_t n) { return n - this->nb_vertices_; }

  std::size_t Graph::weight_node(std::size_t n)
  {
    auto triplet = this->mst[this->get_edge(n)];

    return std::get<2>(triplet);
  }

  static bool sortbyth(const std::tuple<std::size_t, std::size_t, std::size_t>& a,
                       const std::tuple<std::size_t, std::size_t, std::size_t>& b)
  {
    return (std::get<2>(a) < std::get<2>(b));
  }

  void Graph::kruskal()
  {
    for (std::size_t xi = 0; xi < this->nb_vertices_; xi++)
      this->qebt.make_set(xi);

    std::sort(this->edges_.begin(), this->edges_.end(), sortbyth);

    for (std::size_t i = 0; i < this->edges_.size(); i++)
    {
      auto edge = this->edges_[i];

      std::size_t cx = this->qebt.find_canonical(std::get<0>(edge));
      std::size_t cy = this->qebt.find_canonical(std::get<1>(edge));

      if (cx != cy)
        this->qebt.make_union(cx, cy);
      this->mst.push_back(edge);
    }
  }
} // namespace mln