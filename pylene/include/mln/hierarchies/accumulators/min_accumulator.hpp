#pragma once

#include "mln/hierarchies/accumulators/hierarchy_accumulator_base.hpp"

namespace mln
{
  class MinAccumulator : public HierarchyAccumulatorBase<int>
  {
  public:
    explicit MinAccumulator()
      // Neutral element
      : acc_(std::numeric_limits<int>::max())
    {
    }

    ~MinAccumulator() override = default;

    inline void init(int n) override { acc_ = n; }

    inline void invalidate() override { acc_ = -1; }

    inline void merge(MinAccumulator& other) { acc_ = std::min(acc_, other.get_value()); }

    inline int get_value() const override { return acc_; }

  private:
    using HierarchyAccumulatorBase<int>::merge;

    int acc_;
  };

} // namespace mln