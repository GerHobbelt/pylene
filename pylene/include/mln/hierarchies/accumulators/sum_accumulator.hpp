#pragma once

#include "mln/hierarchies/accumulators/hierarchy_accumulator_base.hpp"

namespace mln
{
  template <typename T>
  class SumAccumulator : public HierarchyAccumulatorBase<T>
  {
  public:
    explicit SumAccumulator()
      // Neutral element
      : acc_()
    {
    }

    ~SumAccumulator() override = default;

    inline void init(T n) override { acc_ = n; }

    inline void invalidate() override { acc_ = -1; }

    inline void merge(SumAccumulator& other) { acc_ += other.get_value(); }

    inline T get_value() const override { return acc_; }

  private:
    using HierarchyAccumulatorBase<T>::merge;

    T acc_;
  };

} // namespace mln