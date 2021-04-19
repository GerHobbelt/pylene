#pragma once

#include <utility>

#include "mln/hierarchies/accumulators/hierarchy_accumulator_base.hpp"

namespace mln
{
  // V(n) = area(n) ∗ |altitude(n) − altitude(parent(n))| + ∑c∈children(n)V(c)
  class VolumeAccumulator : public HierarchyAccumulatorBase<std::tuple<int, int>, int>
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
      volume_sum_    = -1;
      diff_altitude_ = -1;
    }

    inline void take(std::tuple<int, int> t) override
    {
      area_.take(std::get<0>(t));
      diff_altitude_ = std::get<1>(t);
    }

    inline void take(const VolumeAccumulator& other)
    {
      area_.take(other.area_);
      volume_sum_ += other.get_value();
    }

    inline int get_value() const override { return area_.get_value() * diff_altitude_ + volume_sum_; }

  private:
    using HierarchyAccumulatorBase<std::tuple<int, int>, int>::take;

    int volume_sum_;
    int diff_altitude_;

    SumAccumulator<int> area_;
  };

} // namespace mln