#pragma once

#include <utility>

#include "mln/hierarchies/accumulators/hierarchy_accumulator_base.hpp"

namespace mln
{
  class VolumeAccumulator : public HierarchyAccumulatorBase<int>
  {
  public:
    explicit VolumeAccumulator(std::function<int(int)> diff_altitude)
      : acc_(0)
      , diff_altitude_(std::move(diff_altitude))
    {
    }

    ~VolumeAccumulator() override = default;

    inline void init() override
    {
      acc_ = 0;
      area_.init();
    }

    inline void invalidate() override
    {
      acc_ = -1;
      area_.invalidate();
    }

    inline void merge(VolumeAccumulator& other)
    {
      area_.merge(other.area_);

      int node = other.get_associated_node();
      other.acc_ += other.area_.get_value() * diff_altitude_(node);

      acc_ += other.get_value();
    }

    inline int get_value() const override { return acc_; }

  private:
    using HierarchyAccumulatorBase<int>::merge;

    int acc_;

    std::function<int(int)> diff_altitude_;

    AreaAccumulator area_;
  };

} // namespace mln