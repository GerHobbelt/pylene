#pragma once

#include <utility>

#include "mln/hierarchies/accumulators/hierarchy_accumulator_base.hpp"

namespace mln
{
  class ExtremaAccumulator : public HierarchyAccumulatorBase<std::tuple<bool, bool>, int>
  {
  public:
    explicit ExtremaAccumulator()
      // Neutral element
      : acc_(true)
      , parent_same_altitude_(true)
    {
    }

    ~ExtremaAccumulator() override = default;

    inline void invalidate() override { acc_ = -1; }

    inline void take(std::tuple<bool, bool> t) override
    {
      acc_                  = std::get<0>(t);
      parent_same_altitude_ = std::get<1>(t);
    }

    inline void take(const ExtremaAccumulator& other) { acc_ &= other.parent_same_altitude_ && other.acc_; }

    inline int get_value() const override
    {
      if (acc_ == -1)
        return -1;

      return acc_ && !parent_same_altitude_;
    }

  private:
    using HierarchyAccumulatorBase<std::tuple<bool, bool>, int>::take;

    int acc_;

    bool parent_same_altitude_;
  };

} // namespace mln