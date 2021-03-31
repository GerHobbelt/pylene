#include "mln/hierarchies/graph.hpp"

namespace mln
{
  void Graph::add_edge(int source, int destination, int weight)
  {
    this->edges_.emplace_back(std::make_tuple(source, destination, weight));
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

  QEBT* Graph::kruskal()
  {
    QEBT* qebt = new QEBT(this);

    for (int xi = 0; xi < this->nb_vertices_; xi++)
      qebt->make_set(xi);

    std::sort(this->edges_.begin(), this->edges_.end(), sortbyth);

    for (std::size_t i = 0; i < this->edges_.size(); i++)
    {
      auto edge = this->edges_[i];

      int cx = qebt->find_canonical(std::get<0>(edge));
      int cy = qebt->find_canonical(std::get<1>(edge));

      if (cx != cy)
      {
        qebt->make_union(cx, cy);
        this->mst.emplace_back(edge);
      }
    }

    return qebt;
  }
} // namespace mln