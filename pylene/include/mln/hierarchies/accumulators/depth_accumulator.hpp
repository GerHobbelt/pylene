#pragma once

#include "mln/hierarchies/accumulators/accumulator_base.hpp"

namespace mln
{
  class DepthAccumulator : public AccumulatorBase<int>
  {
  public:
    DepthAccumulator()
      // Neutral element
      : acc_(-1)
    {
    }

    inline void init() override { acc_ = 0; }

    // FIXME Replace AccumulatorBase<int> by DepthAccumulator
    inline void take(const AccumulatorBase<int>& acc) override { acc_ = acc.extract_value() + 1; }

    inline int extract_value() const override { return acc_; }

  private:
    int acc_;
  };

} // namespace mln