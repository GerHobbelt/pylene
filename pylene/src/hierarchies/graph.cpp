#include "mln/hierarchies/graph.hpp"

namespace mln
{
  void Graph::add_edge(int source, int destination, int weight)
  {
    auto triplet = std::make_tuple(source, destination, weight);
    this->edges_.push_back(triplet);
  }

  int Graph::get_edge(int n) const { return n - this->nb_vertices_; }

  int Graph::weight_node(int n) const
  {
    auto triplet = this->mst[this->get_edge(n)];

    return std::get<2>(triplet);
  }

  static bool sortbyth(const std::tuple<int, int, int>& a, const std::tuple<int, int, int>& b)
  {
    return (std::get<2>(a) < std::get<2>(b));
  }

  const QEBT& Graph::kruskal()
  {
    for (int xi = 0; xi < this->nb_vertices_; xi++)
      this->qebt.make_set(xi);

    std::sort(this->edges_.begin(), this->edges_.end(), sortbyth);

    for (std::size_t i = 0; i < this->edges_.size(); i++)
    {
      auto edge = this->edges_[i];

      int cx = this->qebt.find_canonical(std::get<0>(edge));
      int cy = this->qebt.find_canonical(std::get<1>(edge));

      if (cx != cy)
      {
        this->qebt.make_union(cx, cy);
        this->mst.push_back(edge);
      }
    }

    return this->qebt;
  }
} // namespace mln