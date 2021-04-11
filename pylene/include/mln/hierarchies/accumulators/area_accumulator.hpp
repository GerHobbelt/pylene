#pragma once

#include "mln/hierarchies/accumulators/accumulator_base.hpp"

namespace mln
{
  class AreaAccumulator : public AccumulatorBase<int>
  {
  public:
    AreaAccumulator()
      // Neutral element
      : acc_(0)
    {
    }

    inline void init() override { acc_ = 1; }

    // FIXME Replace AccumulatorBase<int> by AreaAccumulator
    inline void take(const AccumulatorBase<int>& acc) override { acc_ += acc.extract_value(); }

    inline int extract_value() const override { return acc_; }

  private:
    int acc_;
  };

} // namespace mln