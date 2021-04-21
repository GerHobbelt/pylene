#pragma once

#include <type_traits>

namespace mln
{
  /**
   * Interface for accumulators
   *
   * @tparam I Input type used for initialization of accumulators
   * @tparam O Output type that is the type of the returned value
   */
  template <typename I, typename O>
  class HierarchyAccumulatorBase
  {
  public:
    HierarchyAccumulatorBase<I, O>() = default;

    virtual ~HierarchyAccumulatorBase() = default;

    /**
     * Invalidate the accumulator in order to return an invalid value if the corresponding node has been deleted
     */
    virtual void invalidate() = 0;

    /**
     * Method used to initialize the accumulator
     */
    virtual void take(I) = 0;

    /**
     * Method used to merge two accumulators
     * This function must be overloaded in order to specialize the merge between two same type accumulators
     */
    virtual void take(const HierarchyAccumulatorBase<I, O>&){};

    /**
     * @return The value at the end of the accumulation
     */
    virtual O get_value() const = 0;
  };

  template <class T, typename I, typename O>
  concept Accumulator = std::is_base_of<HierarchyAccumulatorBase<I, O>, T>::value;

  template <typename T>
  concept Attribute = std::is_same<T, int>::value or std::is_same<T, double>::value;
} // namespace mln