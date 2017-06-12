#ifndef MLN_CORE_CONCEPT_ITERATOR_HPP
# define MLN_CORE_CONCEPT_ITERATOR_HPP

# include <mln/core/concept/object.hpp>
# include <mln/core/concept/check.hpp>
# include <boost/concept_check.hpp>

namespace mln
{

  namespace details
  {
    // Fwd declaration
    template <typename I>
    struct Iterator;

    template <typename I>
    struct MultiDimensionalIterator;
  }

  template <typename I>
  struct Iterator : Object<I>
  {
    BOOST_CONCEPT_USAGE(Iterator)
    {
      BOOST_CONCEPT_ASSERT((details::Iterator<I>));
    }
  };

  namespace details
  {
    // Real concept here.
    template <typename I>
    struct Iterator
    {
      using value_type = typename I::value_type;
      using reference = typename I::reference;
      using has_NL = typename I::has_NL;
      using is_multidimensional = typename I::is_multidimensional;

      BOOST_CONCEPT_USAGE(Iterator)
      {
        // Remove concept because lambda not assignable
        // BOOST_CONCEPT_ASSERT((boost::Assignable<I>));
        // BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<I>));

        reference (I::*method)() const = &I::operator*;
        (void)method;
        void (I::*method1)() = &I::next;
        (void)method1;
        void (I::*method2)() = &I::init;
        (void)method2;
        bool (I::*method3)() const = &I::finished;
        (void)method3;
        I (I::*method4)() const = &I::iter;
        (void)method4;

        MLN_CONCEPT_ASSERT_IF(is_multidimensional::value, MultiDimensionalIterator<I>);
      }
    };

    template <typename I>
    struct MultiDimensionalIterator
    {
      BOOST_CONCEPT_USAGE(MultiDimensionalIterator)
      {
        void (I::*__inner_init)() = &I::__inner_init;
        void (I::*__inner_next)() = &I::__inner_next;
        bool (I::*__inner_finished)() const = &I::__inner_finished;

        void (I::*__outer_init)() = &I::__outer_init;
        void (I::*__outer_next)() = &I::__outer_next;
        bool (I::*__outer_finished)() const = &I::__outer_finished;

        (void)__inner_init;
        (void)__inner_next;
        (void)__inner_finished;
        (void)__outer_init;
        (void)__outer_next;
        (void)__outer_finished;
      }
    };
  }

} // end of namespace mln

#endif //!MLN_CORE_CONCEPT_ITERATOR_HPP
