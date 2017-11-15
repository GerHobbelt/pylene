#ifndef MLN_CORE_ITERATOR_TRANSFORM_ITERATOR_HPP
#define MLN_CORE_ITERATOR_TRANSFORM_ITERATOR_HPP

#include <mln/core/iterator/iterator_base.hpp>
#include <type_traits>

namespace mln
{

  template <class Iter, class UnaryFunction>
  struct transform_iterator;

  namespace details
  {
    template <class Iter, class UnaryFunction>
    struct transform_iterator_helper
    {
      using reference = std::result_of_t<UnaryFunction(typename Iter::reference)>;
      using value_type = std::remove_reference_t<reference>;
    };

    template <class Iter, class UnaryFunction, typename __is_multidimensional = typename Iter::is_multidimensional>
    struct transform_iterator_base;

    template <class Iter, class UnaryFunction>
    struct transform_iterator_base<Iter, UnaryFunction, std::false_type>
        : iterator_base<transform_iterator<Iter, UnaryFunction>,
                        typename transform_iterator_helper<Iter, UnaryFunction>::value_type,
                        typename transform_iterator_helper<Iter, UnaryFunction>::reference>

    {
      using has_NL = typename Iter::has_NL;
      using is_multidimensional = std::false_type;
      using reference = typename transform_iterator_helper<Iter, UnaryFunction>::reference;

      transform_iterator_base() = default;

      transform_iterator_base(const Iter& it, const UnaryFunction& f) : it_(it), f_(f) {}

      template <class Iterator2, class UnaryFunction2>
      transform_iterator_base(const transform_iterator<Iterator2, UnaryFunction2>& other) : it_(other.it_), f_(other.f_)
      {
      }

      void init() { it_.init(); }
      void next() { it_.next(); }
      bool finished() const { return it_.finished(); }

      reference dereference() const { return f_(*it_); }

      template <class dummy = bool>
      typename std::enable_if<has_NL::value, dummy>::type NL() const
      {
        return it_.NL();
      }

    protected:
      template <class, class, typename>
      friend struct transform_iterator_base;

      Iter it_;
      UnaryFunction f_;
    };

    template <typename Iter, typename UnaryFunction>
    struct transform_iterator_base<Iter, UnaryFunction, std::true_type>
        : transform_iterator_base<Iter, UnaryFunction, std::false_type>
    {
      using transform_iterator_base<Iter, UnaryFunction, std::false_type>::transform_iterator_base;
      using is_multidimensional = std::true_type;

      void __inner_init() { this->it_.__inner_init(); }
      void __inner_next() { this->it_.__inner_next(); }
      bool __inner_finished() const { return this->it_.__inner_finished(); }

      void __outer_init() { this->it_.__outer_init(); }
      void __outer_next() { this->it_.__outer_next(); }
      bool __outer_finished() const { return this->it_.__outer_finished(); }
    };
  }

  template <class Iter, class UnaryFunction>
  struct transform_iterator : details::transform_iterator_base<Iter, UnaryFunction>
  {
    using details::transform_iterator_base<Iter, UnaryFunction>::transform_iterator_base;
  };

  template <typename Iterator, typename UnaryFunction>
  transform_iterator<Iterator, UnaryFunction> make_transform_iterator(const Iterator& it, const UnaryFunction& f)
  {
    return transform_iterator<Iterator, UnaryFunction>(it, f);
  }

} // end of namespace mln

#endif //! MLN_CORE_ITERATOR_TRANSFORM_ITERATOR_HPP
