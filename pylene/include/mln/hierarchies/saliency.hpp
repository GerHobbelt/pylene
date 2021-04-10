#pragma once

#include "mln/core/image/ndbuffer_image.hpp"
#include "mln/core/image/ndimage_fwd.hpp"
#include "mln/hierarchies/graph.hpp"
#include "mln/hierarchies/hierarchy_tree.hpp"

namespace mln
{
  std::vector<Edge> saliency_map(const HierarchyTree& tree);

  image2d<int> saliency_khalimsky_grid(const HierarchyTree& tree);

} // namespace mln