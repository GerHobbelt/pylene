#ifndef MLN_CORE_UTILS_WRAPPER_HPP
#define MLN_CORE_UTILS_WRAPPER_HPP

#include <utility>

/// \brief Set of classes used to manage plain value, reference_wrapper and iterator
/// through a single interface

namespace mln
{

  /// \brief An iterator proxy is much like a reference wrapper that holds
  /// internaly a pointer to the iterator and is able to cast to the value type
  /// through the iterator deferencement.
  template <class Iterator>
  struct iterator_proxy_wrapper
  {
    using type = typename Iterator::value_type;
    using reference = typename Iterator::reference;

    iterator_proxy_wrapper(Iterator& ref) : m_x(&ref) {}
    iterator_proxy_wrapper(Iterator&& ref) = delete;

    operator reference() const { return *(*m_x); }
    reference get() const { return *(*m_x); }

  private:
    Iterator* m_x;
  };

  template <class Iterator>
  iterator_proxy_wrapper<Iterator> make_iterator_proxy(Iterator& x)
  {
    return {x};
  }

  /// \brief A value wrapper is much like a reference wrapper that holds
  /// internaly a copy of the value and is able to cast to the value type
  template <class T>
  struct value_wrapper
  {
    using type = T;
    using reference = T&;

    value_wrapper(const T& v) : m_x(v) {}
    value_wrapper(T&& v) : m_x(std::move(v)) {}

    operator reference() const { return m_x; }
    reference get() const { return m_x; }

  private:
    T m_x;
  };

  template <class T>
  value_wrapper<T> make_value_wrapper(const T& x)
  {
    return {x};
  }

  template <class T>
  value_wrapper<T> make_value_wrapper(T&& x)
  {
    return {std::move(x)};
  }

  template <class Wrapper>
  using unwrap_t = typename Wrapper::type;

  template <class Wrapper>
  using unwrap_ref_t = typename Wrapper::reference;
}

#endif // ! MLN_CORE_UTILS_WRAPPER_HPP
