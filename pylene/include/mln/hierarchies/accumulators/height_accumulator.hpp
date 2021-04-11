#pragma once

#include <utility>

#include "mln/hierarchies/accumulators/hierarchy_accumulator_base.hpp"

namespace mln
{
  class HeightAccumulator : public HierarchyAccumulatorBase<int>
  {
  public:
    explicit HeightAccumulator(std::function<int(int)> parent_altitude)
      // Neutral element
      : acc_(std::numeric_limits<int>::max())
      , parent_altitude_(std::move(parent_altitude))
    {
    }

    ~HeightAccumulator() override = default;

    inline void init() override { acc_ = parent_altitude_(get_associated_node()); }

    inline void invalidate() override { acc_ = -1; }

    inline void merge(HierarchyAccumulatorBase<int>& other) override
    {
      auto& other_height = dynamic_cast<HeightAccumulator&>(other);
      acc_               = std::min(acc_, other_height.acc_);
    }

    inline int get_value() const override { return parent_altitude_(get_associated_node()) - acc_; }

  private:
    int acc_;

    std::function<int(int)> parent_altitude_;
  };

} // namespace mln