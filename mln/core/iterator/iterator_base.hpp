#ifndef MLN_CORE_ITERATOR_ITERATOR_BASE_HPP
# define MLN_CORE_ITERATOR_ITERATOR_BASE_HPP

# include <boost/utility.hpp>
# include <type_traits>

namespace mln
{

  struct use_default {};

  namespace internal
  {

    template <typename Reference>
    struct pointer_wrapper
    {
      explicit pointer_wrapper(Reference x) : x_ (x) {}
      Reference* operator->() { return boost::addressof(x_); }
      operator Reference* () { return boost::addressof(x_); }
    private:
      Reference x_;
    };


    template <typename T>
    inline
    T* make_pointer(T& x)
    {
      return boost::addressof(x);
    }

    template <typename Reference>
    inline
    pointer_wrapper<Reference>
    make_pointer(Reference x)
    {
      return pointer_wrapper<Reference> (x);
    }

  };


  ///
  /// Helper class for iterators
  ///
  template <typename Derived, typename Value,
            typename Reference = Value&>
  struct iterator_base
  {
    typedef typename std::remove_const<Value>::type value_type;
    typedef Reference reference;
    typedef typename std::conditional< std::is_reference<Reference>::value,
				       typename std::remove_reference<Reference>::type*,
				       internal::pointer_wrapper<Reference> >::type pointer;

    Derived iter() const
    {
      return *(this->derived());
    }

    reference
    operator* () const
    {
      return this->derived()->dereference();
    }

    pointer
    operator-> () const
    {
      return internal::make_pointer(this->derived()->dereference());
    }

  private:
    const Derived* derived() const
    {
      return static_cast<const Derived*>(this);
    }

    Derived* derived()
    {
      return static_cast<Derived*>(this);
    }

  };

} // end of namespace mln

#endif //!MLN_CORE_ITERATOR_ITERATOR_BASE_HPP
