#pragma once

#include <utility>

#include "mln/hierarchies/accumulators/hierarchy_accumulator_base.hpp"
#include "mln/hierarchies/accumulators/min_accumulator.hpp"

namespace mln
{
  class HeightAccumulator : public HierarchyAccumulatorBase<std::tuple<int, int>, int>
  {
  public:
    explicit HeightAccumulator()
      // Neutral element
      : parent_altitude_(0)
    {
    }

    ~HeightAccumulator() override = default;

    inline void invalidate() override { acc_.invalidate(); }

    inline void take(std::tuple<int, int> t) override
    {
      acc_.take(std::get<0>(t));
      parent_altitude_ = std::get<1>(t);
    }

    inline void take(const HeightAccumulator& other) { acc_.take(other.acc_); }

    inline int get_value() const override { return parent_altitude_ - acc_.get_value(); }

  private:
    using HierarchyAccumulatorBase<std::tuple<int, int>, int>::take;

    MinAccumulator acc_;

    int parent_altitude_;
  };

} // namespace mln