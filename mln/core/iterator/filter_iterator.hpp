#ifndef MLN_CORE_ITERATOR_FILTER_ITERATOR_HPP
#define MLN_CORE_ITERATOR_FILTER_ITERATOR_HPP

#include <mln/core/iterator/iterator_base.hpp>

#include <type_traits>

namespace mln
{

  template <typename Iterator_, typename Predicate>
  struct filter_iterator;

  template <typename Iterator, typename Predicate>
  filter_iterator<Iterator, Predicate> make_filter_iterator(const Iterator& it, const Predicate& pred);

  /*********************/
  /*** Implementation  */
  /*********************/

  template <typename Iterator_, typename Predicate>
  struct filter_iterator : iterator_base<filter_iterator<Iterator_, Predicate>, typename Iterator_::value_type,
                                         typename Iterator_::reference>
  {
    typedef typename Iterator_::value_type value_type;
    typedef typename Iterator_::reference reference;

    filter_iterator() {}

    filter_iterator(const Iterator_& iterator, const Predicate& pred) : m_it(iterator), m_pred(pred) {}

    template <typename Other>
    filter_iterator(const filter_iterator<Other, Predicate>& other,
                    typename std::enable_if<std::is_convertible<Other, Iterator_>::value>::type* = NULL)
        : m_it(other.m_it), m_pred(other.m_pred)
    {
    }

    void init()
    {
      m_it.init();
      if (not m_pred(*m_it))
        this->next();
    }

    void next()
    {
      m_it.next();
      while (not m_it.finished() and not m_pred(*m_it))
        m_it.next();
    }

    bool finished() const { return m_it.finished(); }

    reference dereference() const { return *m_it; }

  private:
    template <typename, typename>
    friend struct filter_iterator;

    Iterator_ m_it;
    Predicate m_pred;
  };

  template <typename Iterator, typename Predicate>
  filter_iterator<Iterator, Predicate> make_filter_iterator(const Iterator& it, const Predicate& pred)
  {
    return filter_iterator<Iterator, Predicate>(it, pred);
  }
}

#endif // ! MLN_CORE_ITERATOR_FILTER_ITERATOR_HPP
