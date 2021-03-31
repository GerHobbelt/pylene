#pragma once

#include "mln/hierarchies/hierarchy_tree.hpp"

namespace mln
{
  int* depth_attribute(const HierarchyTree& tree);

  int* area_attribute(const HierarchyTree& tree);

  int* volume_attribute(const HierarchyTree& tree);

} // namespace mln