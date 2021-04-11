#pragma once

#include "mln/hierarchies/accumulators/hierarchy_accumulator_base.hpp"
#include "mln/hierarchies/hierarchy_tree.hpp"

#include <vector>

namespace mln
{
  struct HierarchyTraversal
  {
    enum Order
    {
      LEAVES_TO_ROOT = 0,
      ROOT_TO_LEAVES,
    } order = LEAVES_TO_ROOT;

    bool exclude_leaves = false;
  };

  std::vector<int> hierarchy_traversal(const HierarchyTree& tree, const HierarchyTraversal& traversal);

  template <typename T, Accumulator<T> AccumulatorType>
  std::vector<T> compute_attribute_from_accumulator(const HierarchyTree& tree, const AccumulatorType& acc,
                                                    const HierarchyTraversal& traversal);

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