#pragma once

#include "mln/core/colors.hpp"
#include "mln/core/image/ndbuffer_image.hpp"
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

  Graph watershed_graph(Graph& graph, WatershedAttribute attribute_type);

  /**
   * @return The nearest connected component root for each node
   */
  std::vector<int> threshold_cut_labelization(const HierarchyTree& tree, double threshold);

  std::vector<rgb8> mean_color_per_node(const HierarchyTree& tree, const image2d<rgb8>& image);

  image2d<rgb8> hierarchical_segmentation(const mln::image2d<rgb8>& image, WatershedAttribute attribute_type,
                                          double threshold);
} // namespace mln