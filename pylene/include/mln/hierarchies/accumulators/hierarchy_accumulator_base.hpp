#pragma once

#include <type_traits>

namespace mln
{
  template <typename I, typename O>
  class HierarchyAccumulatorBase
  {
  public:
    HierarchyAccumulatorBase<I, O>() = default;

    virtual ~HierarchyAccumulatorBase() = default;

    virtual void invalidate() = 0;

    virtual void take(I) = 0;

    virtual void take(const HierarchyAccumulatorBase<I, O>&){};

    virtual O get_value() const = 0;
  };

  template <class T, typename I, typename O>
  concept Accumulator = std::is_base_of<HierarchyAccumulatorBase<I, O>, T>::value;

  template <typename T>
  concept Attribute = std::is_same<T, int>::value or std::is_same<T, double>::value;
} // namespace mln