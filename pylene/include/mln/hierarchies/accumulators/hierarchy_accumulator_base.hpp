#pragma once

#include <type_traits>

namespace mln
{
  template <typename T>
  class HierarchyAccumulatorBase
  {
  public:
    explicit HierarchyAccumulatorBase<T>()
      : associated_node_(-1)
    {
    }

    virtual ~HierarchyAccumulatorBase() = default;

    virtual void init() = 0;

    virtual void invalidate() = 0;

    inline int get_associated_node() const { return associated_node_; }

    inline void set_associated_node(int associated_node) { associated_node_ = associated_node; }

    virtual void merge(HierarchyAccumulatorBase<T>& other) = 0;

    virtual T get_value() const = 0;

  private:
    int associated_node_;
  };

  template <class T, class U>
  concept Accumulator = std::is_base_of<HierarchyAccumulatorBase<U>, T>::value;
} // namespace mln