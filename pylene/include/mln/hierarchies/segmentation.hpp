#pragma once

#include "mln/hierarchies/graph.hpp"

namespace mln
{
  enum WatershedAttribute
  {
    AREA = 0,
    DYNAMIC,
    HEIGHT,
    DEPTH
  };

  Graph* watershed_graph(Graph* graph, WatershedAttribute attribute_type);

  /**
   * @return The nearest connected component root for each node
   */
  std::vector<int> threshold_cut_labelization(const HierarchyTree& tree, double threshold);
} // namespace mln