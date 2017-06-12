#ifndef MLN_CORE_ITERATOR_ITERATOR_BASE_HPP
# define MLN_CORE_ITERATOR_ITERATOR_BASE_HPP

# include <mln/core/concept/iterator.hpp>
# include <mln/core/iterator/iterator_traits.hpp>
# include <memory>
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
      Reference* operator->() { return std::addressof(x_); }
      operator Reference* () { return std::addressof(x_); }
    private:
      Reference x_;
    };

    template <typename T>
    struct make_pointer
    {
      typedef pointer_wrapper<T> type;
      static pointer_wrapper<T> foo(T x) { return pointer_wrapper<T> (x); }
    };

    template <typename T>
    struct make_pointer<T&>
    {
      typedef T* type;
      static T* foo(T& x) { return std::addressof (x); }
    };

  };


  ///
  /// Helper class for iterators
  ///
  template <typename Derived, typename Value, typename Reference = Value&>
  struct iterator_base : Iterator<Derived>
  {
    using is_multidimensional = std::false_type;
    using iterator = Derived;
    using const_iterator = Derived;
    using has_NL = std::false_type;
    using value_type = std::remove_const_t<Value>;
    using reference = Reference;
    using pointer = std::conditional_t<std::is_reference<Reference>::value, std::remove_reference_t<Reference>*,
                                       internal::pointer_wrapper<Reference>>;

    Derived& iter()
    {
      return *(exact(this));
    }

    Derived iter() const
    {
      return *(exact(this));
    }


    reference
    operator* () const
    {
      // It may not be necessary to start iteration to have valid
      // member (e.g. the image pointer in pixels), thus we should
      // assert this:
      // mln_precondition(not this->derived()->finished());
      return exact(this)->dereference();
    }

    pointer
    operator-> () const
    {
      return internal::make_pointer<reference>::foo(exact(this)->dereference());
    }
  };

  template <typename Derived, typename Value,
            typename Reference = Value&>
  struct multidimensional_iterator_base : iterator_base<Derived, Value, Reference>
  {
    using is_multidimensional = std::true_type;


    void init()
    {
      exact(this)->__outer_init();
      if (not exact(this)->__outer_finished())
        exact(this)->__inner_init();
    }

    void next()
    {
      exact(this)->__inner_next();
      if (exact(this)->__inner_finished())
        {
          exact(this)->__outer_next();
          if (not exact(this)->__outer_finished())
            exact(this)->__inner_init();
        }
    }

    bool finished() const
    {
      return exact(this)->__outer_finished();
    }

  };

} // end of namespace mln

#endif //!MLN_CORE_ITERATOR_ITERATOR_BASE_HPP
