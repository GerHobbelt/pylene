#pragma once

#include "mln/hierarchies/accumulators/hierarchy_accumulator_base.hpp"

namespace mln
{
  template <Attribute T>
  class MaxAccumulator : public HierarchyAccumulatorBase<T, T>
  {
  public:
    explicit MaxAccumulator()
      // Neutral element
      : acc_(std::numeric_limits<T>::min())
    {
    }

    ~MaxAccumulator() override = default;

    inline void invalidate() override { acc_ = -1; }

    inline void take(T n) override { acc_ = n; }

    inline void take(const MaxAccumulator& other) { acc_ = std::max(acc_, other.get_value()); }

    inline T get_value() const override { return acc_; }

  private:
    using HierarchyAccumulatorBase<T, T>::take;

    T acc_;
  };

} // namespace mln