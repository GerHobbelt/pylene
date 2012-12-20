#ifndef MLN_CORE_CONCEPT_ITERATOR_HPP
# define MLN_CORE_CONCEPT_ITERATOR_HPP

# include <boost/concept_check.hpp>

namespace mln
{

  template <typename I>
  struct Iterator
  {

    Iterator()
    {
      // Remove concept because lambda not assignable
      //BOOST_CONCEPT_ASSERT((boost::Assignable<I>));
      //BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<I>));

      typedef typename I::value_type    value_type;
      typedef typename I::reference     reference;

      reference (I::*method) () const = &I::operator*;
      (void) method;
      void (I::*method1) () = &I::next;
      (void) method1;
      void (I::*method2) () = &I::init;
      (void) method2;
      bool (I::*method3) () const = &I::finished;
      (void) method3;
      I (I::*method4) () const = &I::iter;
      (void) method4;
    }
  };

} // end of namespace mln

#endif //!MLN_CORE_CONCEPT_ITERATOR_HPP
