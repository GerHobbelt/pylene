#pragma once

#include <utility>

#include "mln/hierarchies/accumulators/hierarchy_accumulator_base.hpp"

namespace mln
{
  class ExtremaAccumulator : public HierarchyAccumulatorBase<int>
  {
  public:
    explicit ExtremaAccumulator(std::function<bool(int)> same_altitude)
      : acc_(1)
      , same_altitude_(std::move(same_altitude))
    {
    }

    ~ExtremaAccumulator() override = default;

    inline void init() override { acc_ = 0; }

    inline void invalidate() override { acc_ = -1; }

    inline void merge(HierarchyAccumulatorBase<int>& other) override
    {
      auto& other_extrema = dynamic_cast<ExtremaAccumulator&>(other);

      int node = other_extrema.get_associated_node();
      acc_ &= same_altitude_(node) && other_extrema.get_value();

      other_extrema.acc_ &= !same_altitude_(node);
    }

    inline int get_value() const override { return acc_; }

  private:
    int acc_;

    std::function<bool(int)> same_altitude_;
  };

} // namespace mln