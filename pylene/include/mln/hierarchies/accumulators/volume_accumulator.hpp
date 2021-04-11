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

    inline void merge(HierarchyAccumulatorBase<int>& other) override
    {
      auto& other_volume = dynamic_cast<VolumeAccumulator&>(other);

      area_.merge(other_volume.area_);

      int node = other_volume.get_associated_node();
      other_volume.acc_ += other_volume.area_.get_value() * diff_altitude_(node);

      acc_ += other_volume.get_value();
    }

    inline int get_value() const override { return acc_; }

  private:
    int acc_;

    std::function<int(int)> diff_altitude_;

    AreaAccumulator area_;
  };

} // namespace mln