#pragma once

#include "mln/core/image/ndimage_fwd.hpp"
#include "mln/hierarchies/graph.hpp"
#include "mln/hierarchies/hierarchy_tree.hpp"

namespace mln
{
  std::vector<Edge> saliency_map(const HierarchyTree& tree);

  // FIXME Return an image
  int* saliency_khalimsky_grid(const HierarchyTree& tree);

} // namespace mln