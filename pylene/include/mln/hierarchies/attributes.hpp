#pragma once

#include "mln/hierarchies/hierarchy_tree.hpp"

namespace mln
{
  int* depth_attribute(const HierarchyTree& tree);

  int* area_attribute(const HierarchyTree& tree);

  int* volume_attribute(const HierarchyTree& tree);

  /**
   * @return Array of 0/1 for false/true and -1 for deleted nodes
   */
  int* extrema_attribute(const HierarchyTree& tree);

  int* height_attribute(const HierarchyTree& tree);

} // namespace mln