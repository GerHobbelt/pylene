#pragma once

#include "mln/core/image/ndimage.hpp"
#include "mln/hierarchies/accumulators/hierarchy_accumulator_base.hpp"
#include "mln/hierarchies/hierarchy_tree.hpp"

namespace mln
{
  template <Attribute AttributeType, typename ReconstructionType>
  struct HierarchicalSegmentationBase
  {
    virtual std::vector<AttributeType> on_attribute_computation(const HierarchyTree&) { return {}; }

    virtual std::vector<int> on_labelization(const HierarchyTree&, double) { return {}; }

    virtual std::vector<ReconstructionType> on_reconstruction(const HierarchyTree&, image2d<ReconstructionType>)
    {
      return {};
    }
  };

  template <typename T, typename AttributeType, typename O>
  concept HierarchicalSegmentationCanvas = std::is_base_of<HierarchicalSegmentationBase<AttributeType, O>, T>::value;
} // namespace mln