#pragma once

#include <type_traits>

namespace mln
{
  template <typename T>
  struct AccumulatorBase
  {
    virtual void init() = 0;

    virtual void take(T element) = 0;

    virtual void take(const AccumulatorBase<T>& acc) = 0;

    virtual T extract_value() const = 0;
  };

  template <class T, class U>
  concept Accumulator = std::is_base_of<AccumulatorBase<U>, T>::value;
} // namespace mln