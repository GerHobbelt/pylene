#pragma once

namespace mln
{
  class Graph;

  struct HierarchyTree
  {
    explicit HierarchyTree(const Graph& leaf_graph)
      : leaf_graph(leaf_graph)
    {
    }

    virtual ~HierarchyTree() = default;

    virtual int get_nb_vertices() const = 0;

    virtual int get_parent(int node) const = 0;

    const Graph& leaf_graph;
  };

} // namespace mln