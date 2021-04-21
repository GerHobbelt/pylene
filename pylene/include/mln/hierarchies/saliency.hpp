#pragma once

#include "mln/core/image/ndbuffer_image.hpp"
#include "mln/core/image/ndimage_fwd.hpp"
#include "mln/hierarchies/graph.hpp"
#include "mln/hierarchies/hierarchy_tree.hpp"

namespace mln
{
  /**
   * Generate a saliency map of the given Hierarchy Tree
   *
   * @return A list of edges that associates each edges to a saliency weight
   */
  std::vector<Edge> saliency_map(const HierarchyTree& tree);

  /**
   * Produce a visualization of the given Hierarchy Tree using the Khalimsky grid of the saliency map
   */
  image2d<double> saliency_khalimsky_grid(const HierarchyTree& tree);

} // namespace mln