#pragma once

#include "mln/hierarchies/accumulators/hierarchy_accumulator_base.hpp"

namespace mln
{
  class DepthAccumulator : public HierarchyAccumulatorBase<int>
  {
  public:
    explicit DepthAccumulator()
      // Neutral element
      : acc_(-1)
    {
    }

    ~DepthAccumulator() override = default;

    inline void init() override { acc_ = 0; }

    inline void invalidate() override { acc_ = -1; }

    inline void merge(DepthAccumulator& other) { acc_ = other.get_value() + 1; }

    inline int get_value() const override { return acc_; }

  private:
    using HierarchyAccumulatorBase<int>::merge;

    int acc_;
  };

} // namespace mln