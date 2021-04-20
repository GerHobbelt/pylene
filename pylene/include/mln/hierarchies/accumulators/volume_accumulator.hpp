#pragma once

#include <utility>

#include "mln/hierarchies/accumulators/hierarchy_accumulator_base.hpp"

namespace mln
{
  // V(n) = area(n) ∗ |altitude(n) − altitude(parent(n))| + ∑c∈children(n)V(c)
  class VolumeAccumulator : public HierarchyAccumulatorBase<std::tuple<double, double>, double>
  {
  public:
    explicit VolumeAccumulator()
      // Neutral element
      : volume_sum_(0)
      , diff_altitude_(0)
    {
    }

    ~VolumeAccumulator() override = default;

    inline void invalidate() override
    {
      area_.invalidate();
      volume_sum_ = -1;
    }

    inline void take(std::tuple<double, double> t) override
    {
      area_.take(std::get<0>(t));
      diff_altitude_ = std::get<1>(t);
    }

    inline void take(const VolumeAccumulator& other)
    {
      area_.take(other.area_);
      volume_sum_ += other.get_value();
    }

    inline double get_value() const override
    {
      // Invalid accumulator
      if (volume_sum_ == -1)
        return -1;

      return area_.get_value() * diff_altitude_ + volume_sum_;
    }

  private:
    using HierarchyAccumulatorBase<std::tuple<double, double>, double>::take;

    double volume_sum_;
    double diff_altitude_;

    SumAccumulator<double> area_;
  };

} // namespace mln