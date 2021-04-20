#pragma once

#include "mln/hierarchies/accumulators/hierarchy_accumulator_base.hpp"

namespace mln
{
  template <Attribute T>
  class MinAccumulator : public HierarchyAccumulatorBase<T, T>
  {
  public:
    explicit MinAccumulator()
      // Neutral element
      : acc_(std::numeric_limits<T>::max())
    {
    }

    ~MinAccumulator() override = default;

    inline void invalidate() override { acc_ = -1; }

    inline void take(T n) override { acc_ = n; }

    inline void take(const MinAccumulator& other) { acc_ = std::min(acc_, other.get_value()); }

    inline T get_value() const override { return acc_; }

  private:
    using HierarchyAccumulatorBase<T, T>::take;

    T acc_;
  };

} // namespace mln