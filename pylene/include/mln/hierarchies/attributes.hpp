#pragma once

#include "mln/hierarchies/hierarchy_tree.hpp"

#include <vector>

namespace mln
{
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