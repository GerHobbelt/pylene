#ifndef MLN_CORE_RANGE_ITERATOR_RANGE_HPP
# define MLN_CORE_RANGE_ITERATOR_RANGE_HPP

# include <mln/core/concept/iterator.hpp>

namespace mln
{

  template <typename I>
  struct iterator_range
  {
    typedef I iterator;

    iterator_range(const iterator& x)
      : iter_ (x)
    {
    }

    iterator iter() const
    {
      return iter_;
    }

  private:
    I iter_;
  };


  template <typename Iterator1, typename Iterator2>
  struct bidirectional_iterator_range
  {
    typedef Iterator1 iterator;
    typedef Iterator2 reverse_iterator;

    iterator_range(const iterator& x, const reverse_iterator& y)
      : iter_ (x), riter_(y)
    {
    }

    iterator
    iter() const
    {
      return iter_;
    }

    reverse_iterator
    riter() const
    {
      return riter_;
    }

  private:
    iterator iter_;
    reverse_iterator riter_;
  }


} // end of namespace mln

#endif //!MLN_CORE_RANGE_ITERATOR_RANGE_HPP
