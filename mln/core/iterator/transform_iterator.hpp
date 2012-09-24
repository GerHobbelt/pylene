#ifndef MLN_CORE_ITERATOR_TRANSFORM_ITERATOR_HPP
# define MLN_CORE_ITERATOR_TRANSFORM_ITERATOR_HPP

# include <mln/core/iterator/iterator_base.hpp>

namespace mln
{

  namespace internal
  {

    template <typename Iterator, typename UnaryFunction,
              typename Reference, typename Value>
    struct transform_iterator_helper
    {
      typedef typename std::conditional<std::is_same<Reference, use_default>::value,
                                        typename std::result_of< UnaryFunction(Iterator::reference) >::type,
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
    iterator_base< transform_iterator<Iterator, Function>,
                   typename internal::transform_iterator_helper<Iterator, UnaryFunction, Reference, Value>::value_type,
                   typename internal::transform_iterator_helper<Iterator, UnaryFunction, Reference, Value>::reference >
  {
    typedef typename internal::transform_iterator_helper<Iterator, UnaryFunction, Reference, Value>::reference reference;

    transform_iterator(const Iterator& it, const Function& f)
      : it_ (it), f_ (f)
    {
    }

    void init() { it_.init(); }
    void next() { it_.next(); }
    bool finished() const { return it_.finished(); }
    reference dereference() const { return f_(*it_); }

  private:
    Iterator it_;
    Function f_;
  };


} // end of namespace mln

#endif //!MLN_CORE_ITERATOR_TRANSFORM_ITERATOR_HPP
