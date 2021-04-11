#pragma once

#include "mln/hierarchies/accumulators/accumulator_base.hpp"
#include "mln/hierarchies/hierarchy_tree.hpp"

#include <vector>

namespace mln
{
  enum TraversalOrder
  {
    ROOT_TO_LEAVES,
    LEAVES_TO_ROOT
  };

  template <typename T, Accumulator<T> AccumulatorType>
  std::vector<T> compute_attribute_from_accumulator(const HierarchyTree& tree, TraversalOrder traversal_order);

  std::vector<int> depth_attribute(const HierarchyTree& tree);

  std::vector<int> area_attribute(const HierarchyTree& tree);

  std::vector<int> volume_attribute(const HierarchyTree& tree);

  /**
   * @return Vector of 0/1 for false/true and -1 for deleted nodes
   */
  std::vector<int> extrema_attribute(const HierarchyTree& tree);

  std::vector<int> height_attribute(const HierarchyTree& tree);

  std::vector<int> dynamic_attribute(const HierarchyTree& tree);
} // namespace mln