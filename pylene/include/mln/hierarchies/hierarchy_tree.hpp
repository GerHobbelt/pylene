#pragma once

namespace mln
{
  class Graph;

  /**
   * Interface for the Hierarchy Tree returned from kruskal algorithm
   * Collections QEBT/QBT/QT/QCT inherit from it
   */
  struct HierarchyTree
  {
    explicit HierarchyTree(const Graph& leaf_graph)
      : leaf_graph(leaf_graph)
    {
    }

    virtual ~HierarchyTree() = default;

    virtual int get_nb_vertices() const = 0;

    virtual int get_parent(int node) const = 0;

    //! The leaf graph from which the hierarchy tree has been computed
    const Graph& leaf_graph;
  };

} // namespace mln