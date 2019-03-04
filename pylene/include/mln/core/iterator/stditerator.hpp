#pragma once
#include <mln/core/iterator/iterator_base.hpp>

#include <iterator>
#include <type_traits>


namespace mln
{

  template <typename Iterator_>
  struct stditerator : iterator_base<stditerator<Iterator_>, typename std::iterator_traits<Iterator_>::value_type,
                                     typename std::iterator_traits<Iterator_>::reference>
  {
    typedef typename std::iterator_traits<Iterator_>::reference reference;

    stditerator() {}

    stditerator(const Iterator_& begin, const Iterator_& end)
      : begin_(begin)
      , end_(end)
    {
    }

    template <typename Iterator2>
    stditerator(const stditerator<Iterator2>& other,
                typename std::enable_if<std::is_convertible<Iterator2, Iterator_>::value>::type* = NULL)
      : cur_(other.cur_)
      , begin_(other.begin_)
      , end_(other.end_)
    {
    }

    void init() { cur_ = begin_; }

    void next() { ++cur_; }

    bool finished() const { return cur_ == end_; }

    reference dereference() const { return *cur_; }

  private:
    template <typename>
    friend struct stditerator;

    Iterator_ cur_;
    Iterator_ begin_;
    Iterator_ end_;
  };
} // namespace mln
