#ifndef MLN_CORE_ITERATOR_TRANSFORM_ITERATOR_HPP
# define MLN_CORE_ITERATOR_TRANSFORM_ITERATOR_HPP

# include <mln/core/iterator/iterator_base.hpp>
# include <type_traits>

namespace mln
{

  namespace internal
  {

    template <typename Iterator, typename UnaryFunction,
              typename Reference, typename Value>
    struct transform_iterator_helper
    {
      typedef typename std::conditional<std::is_same<Reference, use_default>::value,
                                        typename std::result_of< UnaryFunction(typename Iterator::reference) >::type,
                                        Reference>::type reference;

      typedef typename std::conditional<std::is_same<Value, use_default>::value,
                                        typename std::remove_reference<reference>::type,
                                        Value>::type value_type;
    };

  };

  template <typename Iterator, typename UnaryFunction,
            typename Reference = use_default,
            typename Value = use_default>
  struct transform_iterator :
    iterator_base< transform_iterator<Iterator, UnaryFunction>,
                   typename internal::transform_iterator_helper<Iterator, UnaryFunction, Reference, Value>::value_type,
                   typename internal::transform_iterator_helper<Iterator, UnaryFunction, Reference, Value>::reference >
  {
    typedef typename internal::transform_iterator_helper<Iterator, UnaryFunction, Reference, Value>::reference reference;

    transform_iterator() = default;

    transform_iterator(const Iterator& it, const UnaryFunction& f)
      : it_ (it), f_ (f)
    {
    }

    template <typename Iterator2, typename Reference2, typename Value2>
    transform_iterator(const transform_iterator<Iterator2, UnaryFunction, Reference2, Value2>& other,
		       typename std::enable_if<std::is_convertible<Iterator2, Iterator>::value>::type* = NULL)
      : it_ (other.it_), f_ (other.f_)
    {
    }

    void init() { it_.init(); }
    void next() { it_.next(); }
    bool finished() const { return it_.finished(); }
    reference dereference() const { return f_(*it_); }

  private:
    template <typename, typename, typename, typename>
    friend struct transform_iterator;

    Iterator it_;
    UnaryFunction f_;
  };


} // end of namespace mln

#endif //!MLN_CORE_ITERATOR_TRANSFORM_ITERATOR_HPP
