#pragma once

#include <type_traits>

namespace mln
{
  /**
   * Interface for accumulators
   *
   * @tparam ArgumentType Input type used for initialization of accumulators
   * @tparam ResultType Output type that is the type of the returned value
   */
  template <typename ArgumentType, typename ResultType>
  class HierarchyAccumulatorBase
  {
  public:
    HierarchyAccumulatorBase<ArgumentType, ResultType>() = default;

    virtual ~HierarchyAccumulatorBase() = default;

    /**
     * Invalidate the accumulator in order to return an invalid value if the corresponding node has been deleted
     */
    virtual void invalidate() = 0;

    /**
     * Method used to initialize the accumulator
     */
    virtual void take(ArgumentType) = 0;

    /**
     * Method used to merge two accumulators
     * This function must be overloaded in order to specialize the merge between two same type accumulators
     */
    virtual void take(const HierarchyAccumulatorBase<ArgumentType, ResultType>&){};

    /**
     * @return The value at the end of the accumulation
     */
    virtual ResultType get_value() const = 0;
  };

  template <class T, typename ArgumentType, typename ResultType>
  concept Accumulator = std::is_base_of<HierarchyAccumulatorBase<ArgumentType, ResultType>, T>::value;

  template <typename T>
  concept Attribute = std::is_same<T, int>::value or std::is_same<T, double>::value;
} // namespace mln