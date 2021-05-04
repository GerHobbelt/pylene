#pragma once

#include "mln/core/image/ndbuffer_image.hpp"
#include "mln/core/image/ndimage_fwd.hpp"
#include "mln/hierarchies/graph.hpp"
#include "mln/morpho/component_tree.hpp"

namespace mln
{
  /**
   * Generate a saliency map of the given Component Tree
   *
   * @return A list of edges that associates each edges to a saliency weight
   */
  std::vector<Edge> saliency_map(mln::morpho::component_tree<uint8_t> tree, const auto node_map,
                                 const Graph& leaf_graph);

  /**
   * Produce a visualization of the given Component Tree using the Khalimsky grid of the saliency map
   */
  mln::image2d<double> saliency_khalimsky_grid(mln::morpho::component_tree<uint8_t> tree, const auto node_map,
                                               const Graph& leaf_graph);

} // namespace mln