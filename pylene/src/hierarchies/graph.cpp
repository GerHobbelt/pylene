#include "mln/hierarchies/graph.hpp"

namespace mln
{
    void Graph::add_edge(unsigned source, unsigned destination, unsigned weight)
    {
        auto triplet = std::make_tuple(source, destination, weight);
        this->edges_.push_back(triplet);
    }

    unsigned Graph::get_edge(unsigned n)
    {
        return n - this->nb_vertices_;
    }
    
    unsigned Graph::weight_node(unsigned n)
    {
        auto triplet = this->mst[this->get_edge(n)];

        return std::get<2>(triplet);
    }

    void Graph::kruskal()
    {
        // FIXME
    }
} // namespace mln