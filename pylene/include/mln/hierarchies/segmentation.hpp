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
} // namespace mln