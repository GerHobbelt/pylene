#ifndef MLN_CORE_UTILS_ITERATOR_PROXY_HPP
# define MLN_CORE_UTILS_ITERATOR_PROXY_HPP

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

    operator reference () const { return *(*m_x); }
    reference get() const { return *(*m_x); }

  private:
    Iterator* m_x;
  };


  template <class Iterator>
  iterator_proxy_wrapper<Iterator>
  make_iterator_proxy(Iterator& x)
  {
    return {x};
  }

}

#endif // ! MLN_CORE_UTILS_ITERATOR_PROXY_HPP
