#pragma once

#include <utility>

#include "mln/hierarchies/accumulators/hierarchy_accumulator_base.hpp"

namespace mln
{
  /**
   * \brief Compute deepest altitude node of the tree rooted in the corresponding node
   *        and the node that is on the path toward it from the corresponding node
   */
  class DeepestAltitudeAccumulator : public HierarchyAccumulatorBase<std::tuple<int, int>>
  {
  public:
    explicit DeepestAltitudeAccumulator(std::function<int(int)> internal_altitude)
      // Neutral element
      : acc_(std::numeric_limits<int>::max())
      , node_to_minima_(-1)
      , internal_altitude_(std::move(internal_altitude))
    {
    }

    ~DeepestAltitudeAccumulator() override = default;

    inline void init() override
    {
      acc_            = 0;
      node_to_minima_ = -1;
    }

    inline void invalidate() override
    {
      acc_            = -1;
      node_to_minima_ = -1;
    }

    inline void merge(DeepestAltitudeAccumulator& other)
    {
      int node = other.get_associated_node();
      if (other.acc_ == std::numeric_limits<int>::max())
        other.acc_ = internal_altitude_(node);

      if (other.acc_ < acc_)
      {
        acc_            = other.acc_;
        node_to_minima_ = node;
      }
    }

    inline std::tuple<int, int> get_value() const override { return std::make_tuple(acc_, node_to_minima_); }

  private:
    using HierarchyAccumulatorBase<std::tuple<int, int>>::merge;

    int acc_;
    int node_to_minima_;

    std::function<int(int)> internal_altitude_;
  };
} // namespace mln