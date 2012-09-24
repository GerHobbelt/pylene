#ifndef MLN_CORE_ITERATOR_ITERATOR_BASE_HPP
# define MLN_CORE_ITERATOR_ITERATOR_BASE_HPP

# include <boost/utility.hpp>

namespace mln
{

  namespace internal
  {

    template <typename Reference, typename Pointer>
    struct pointer_wrapper
    {
      explicit pointer_wrapper(Reference x) : x_ (x)
      Reference* operator->() { return boost::adressof(x_); }
      operator Reference* () { return boost::adressof(x_); }
    private:
      Reference x_;
    };


    template <typename T>
    inline
    T* make_pointer(T& x)
    {
      return boost::adressof(x);
    }

    template <typename Reference>
    inline
    pointer_wrapper<Reference>
    make_pointer(Reference x)
    {
      return pointer_wrapper(x);
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
    typedef typename std::result_of< internal::make_pointer(Reference) >::type pointer;

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
