#pragma once

#include <utility>

#include "mln/hierarchies/accumulators/hierarchy_accumulator_base.hpp"

namespace mln
{
  class VolumeAccumulator : public HierarchyAccumulatorBase<int>
  {
  public:
    explicit VolumeAccumulator(std::function<int(int)> diff_altitude)
      // Neutral element
      : acc_(0)
      , diff_altitude_(std::move(diff_altitude))
    {
    }

    ~VolumeAccumulator() override = default;

    inline void init(int n) override
    {
      acc_ = 0;
      sum_.init(n);
    }

    inline void invalidate() override
    {
      acc_ = -1;
      sum_.invalidate();
    }

    inline void merge(VolumeAccumulator& other)
    {
      sum_.merge(other.sum_);

      int node = other.get_associated_node();
      other.acc_ += other.sum_.get_value() * diff_altitude_(node);

      acc_ += other.get_value();
    }

    inline int get_value() const override { return acc_; }

  private:
    using HierarchyAccumulatorBase<int>::merge;

    int acc_;

    std::function<int(int)> diff_altitude_;

    SumAccumulator sum_;
  };

} // namespace mln