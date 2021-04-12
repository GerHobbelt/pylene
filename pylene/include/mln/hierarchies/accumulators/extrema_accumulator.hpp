#pragma once

#include <utility>

#include "mln/hierarchies/accumulators/hierarchy_accumulator_base.hpp"

namespace mln
{
  class ExtremaAccumulator : public HierarchyAccumulatorBase<int>
  {
  public:
    explicit ExtremaAccumulator(std::function<bool(int)> same_altitude)
      // Neutral element
      : acc_(1)
      , same_altitude_(std::move(same_altitude))
    {
    }

    ~ExtremaAccumulator() override = default;

    inline void init(int n) override { acc_ = n; }

    inline void invalidate() override { acc_ = -1; }

    inline void merge(ExtremaAccumulator& other)
    {
      int node = other.get_associated_node();
      acc_ &= same_altitude_(node) && other.get_value();

      other.acc_ &= !same_altitude_(node);
    }

    inline int get_value() const override { return acc_; }

  private:
    using HierarchyAccumulatorBase<int>::merge;

    int acc_;

    std::function<bool(int)> same_altitude_;
  };

} // namespace mln