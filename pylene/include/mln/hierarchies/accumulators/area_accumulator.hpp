#pragma once

#include "mln/hierarchies/accumulators/hierarchy_accumulator_base.hpp"

namespace mln
{
  class AreaAccumulator : public HierarchyAccumulatorBase<int>
  {
  public:
    explicit AreaAccumulator()
      // Neutral element
      : acc_(0)
    {
    }

    ~AreaAccumulator() override = default;

    inline void init() override { acc_ = 1; }

    inline void invalidate() override { acc_ = -1; }

    inline void merge(AreaAccumulator& other) { acc_ += other.get_value(); }

    inline int get_value() const override { return acc_; }

  private:
    using HierarchyAccumulatorBase<int>::merge;

    int acc_;
  };

} // namespace mln