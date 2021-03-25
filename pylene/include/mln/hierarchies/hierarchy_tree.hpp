#pragma once

#include "mln/hierarchies/graph.hpp"

namespace mln
{
    struct HierarchyTree
    {
        virtual std::size_t get_nb_vertices() const = 0;

        virtual std::size_t get_parent(std::size_t node) const = 0;

        const Graph& leaf_graph;
    };

} // namespace mln