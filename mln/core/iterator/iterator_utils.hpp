//---------------------------------------------------------------------------------
// Copyright (c) DxO Labs 2003-2017, this copyright cannot be removed or concealed.
//---------------------------------------------------------------------------------
#ifndef MLN_CORE_ITERATOR_ITERATOR_UTILS_HPP
#define MLN_CORE_ITERATOR_ITERATOR_UTILS_HPP

#include <mln/core/concept/iterator.hpp>

#include <type_traits>

namespace mln
{

  template <class Iter>
  void inner_init(Iterator<Iter>& it);

  template <class Iter>
  void inner_next(Iterator<Iter>& it);

  template <class Iter>
  bool inner_finished(const Iterator<Iter>& it);

  template <class Iter>
  void outer_init(Iterator<Iter>& it);

  template <class Iter>
  void outer_next(Iterator<Iter>& it);

  template <class Iter>
  bool outer_finished(const Iterator<Iter>& it);

  namespace details
  {
    template <class Iter>
    void inner_init(Iter& it, std::true_type /* is_multidimensional */)
    {
      it.__inner_init();
    }

    template <class Iter>
    void inner_init(Iter& it, std::false_type /* is_multidimensional */)
    {
      (void)it;
    }

    template <class Iter>
    void inner_next(Iter& it, std::true_type /* is_multidimensional */)
    {
      it.__inner_next();
    }

    template <class Iter>
    void inner_next(Iter& it, std::false_type /* is_multidimensional */)
    {
      it.next();
    }

    template <class Iter>
    bool inner_finished(const Iter& it, std::true_type /* is_multidimensional */)
    {
      return it.__inner_finished();
    }

    template <class Iter>
    bool inner_finished(const Iter& it, std::false_type /* is_multidimensional */)
    {
      return it.finished();
    }

    template <class Iter>
    void outer_init(Iter& it, std::true_type /* is_multidimensional */)
    {
      it.__outer_init();
    }

    template <class Iter>
    void outer_init(Iter& it, std::false_type /* is_multidimensional */)
    {
      it.init();
    }

    template <class Iter>
    void outer_next(Iter& it, std::true_type /* is_multidimensional */)
    {
      it.__outer_next();
    }

    template <class Iter>
    void outer_next(Iter& it, std::false_type /* is_multidimensional */)
    {
      (void)it;
    }

    template <class Iter>
    bool outer_finished(const Iter& it, std::true_type /* is_multidimensional */)
    {
      return it.__outer_finished();
    }

    template <class Iter>
    bool outer_finished(const Iter& it, std::false_type /* is_multidimensional */)
    {
      return it.finished();
    }
  }

  template <class Iter>
  void inner_init(Iterator<Iter>& it)
  {
    details::inner_init(exact(it), typename Iter::is_multidimensional());
  }

  template <class Iter>
  void inner_next(Iterator<Iter>& it)
  {
    details::inner_next(exact(it), typename Iter::is_multidimensional());
  }

  template <class Iter>
  bool inner_finished(const Iterator<Iter>& it)
  {
    return details::inner_finished(exact(it), typename Iter::is_multidimensional());
  }

  template <class Iter>
  void outer_init(Iterator<Iter>& it)
  {
    details::outer_init(exact(it), typename Iter::is_multidimensional());
  }

  template <class Iter>
  void outer_next(Iterator<Iter>& it)
  {
    details::outer_next(exact(it), typename Iter::is_multidimensional());
  }

  template <class Iter>
  bool outer_finished(const Iterator<Iter>& it)
  {
    return details::outer_finished(exact(it), typename Iter::is_multidimensional());
  }
}

#endif // ! MLN_CORE_ITERATOR_ITERATOR_UTILS_HPP
