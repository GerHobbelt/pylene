#pragma once

#include "mln/hierarchies/accumulators/hierarchy_accumulator_base.hpp"

namespace mln
{
  class SumAccumulator : public HierarchyAccumulatorBase<int>
  {
  public:
    explicit SumAccumulator()
      // Neutral element
      : acc_(0)
    {
    }

    ~SumAccumulator() override = default;

    inline void init(int n) override { acc_ = n; }

    inline void invalidate() override { acc_ = -1; }

    inline void merge(SumAccumulator& other) { acc_ += other.get_value(); }

    inline int get_value() const override { return acc_; }

  private:
    using HierarchyAccumulatorBase<int>::merge;

    int acc_;
  };

} // namespace mln