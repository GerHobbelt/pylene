#pragma once

#include "mln/hierarchies/accumulators/hierarchy_accumulator_base.hpp"

namespace mln
{
  class MaxAccumulator : public HierarchyAccumulatorBase<int, int>
  {
  public:
    explicit MaxAccumulator()
      // Neutral element
      : acc_(std::numeric_limits<int>::min())
    {
    }

    ~MaxAccumulator() override = default;

    inline void invalidate() override { acc_ = -1; }

    inline void take(int n) override { acc_ = n; }

    inline void take(const MaxAccumulator& other) { acc_ = std::max(acc_, other.get_value()); }

    inline int get_value() const override { return acc_; }

  private:
    using HierarchyAccumulatorBase<int, int>::take;

    int acc_;
  };

} // namespace mln