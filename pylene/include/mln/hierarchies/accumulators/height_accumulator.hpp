#pragma once

#include <utility>

#include "mln/hierarchies/accumulators/hierarchy_accumulator_base.hpp"
#include "mln/hierarchies/accumulators/min_accumulator.hpp"

namespace mln
{
  class HeightAccumulator : public HierarchyAccumulatorBase<std::tuple<double, double>, double>
  {
  public:
    explicit HeightAccumulator()
      // Neutral element
      : parent_altitude_(0)
    {
    }

    ~HeightAccumulator() override = default;

    inline void invalidate() override { acc_.invalidate(); }

    inline void take(std::tuple<double, double> t) override
    {
      acc_.take(std::get<0>(t));
      parent_altitude_ = std::get<1>(t);
    }

    inline void take(const HeightAccumulator& other) { acc_.take(other.acc_); }

    inline double get_value() const override
    {
      // Invalid accumulator
      if (acc_.get_value() == -1)
        return -1;

      return parent_altitude_ - acc_.get_value();
    }

  private:
    using HierarchyAccumulatorBase<std::tuple<double, double>, double>::take;

    MinAccumulator<double> acc_;

    double parent_altitude_;
  };

} // namespace mln