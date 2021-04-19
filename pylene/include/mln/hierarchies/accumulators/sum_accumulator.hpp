#pragma once

#include "mln/hierarchies/accumulators/hierarchy_accumulator_base.hpp"

namespace mln
{
  template <typename T>
  class SumAccumulator : public HierarchyAccumulatorBase<T, T>
  {
  public:
    explicit SumAccumulator()
      // Neutral element
      : acc_()
    {
    }

    ~SumAccumulator() override = default;

    inline void invalidate() override { acc_ = -1; }

    inline void take(T n) override { acc_ = n; }

    inline void take(const SumAccumulator& other) { acc_ += other.get_value(); }

    inline T get_value() const override { return acc_; }

  private:
    using HierarchyAccumulatorBase<T, T>::take;

    T acc_;
  };

} // namespace mln