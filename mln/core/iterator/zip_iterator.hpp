#ifndef ZIP_ITERATOP_HPP
# define ZIP_ITERATOP_HPP

# include <type_traits>
# include <mln/core/iterator/iterator_base.hpp>
# include <boost/iterator/zip_iterator.hpp>
# include <boost/tuple/tuple.hpp>

namespace mln
{

  template <typename IteratorTuple>
  struct zip_iterator;


  /********************/
  /* Implementatoin   */
  /********************/

  namespace internal
  {
    using boost::detail::tuple_impl_specific::tuple_meta_transform;
    using boost::detail::tuple_impl_specific::tuple_meta_accumulate;
    using boost::detail::tuple_impl_specific::tuple_transform;
    using boost::detail::tuple_impl_specific::tuple_for_each;

    struct iterator_dereference
    {
      template <typename Iterator>
      struct apply { typedef typename std::remove_reference<Iterator>::type::reference type; };

      template <typename Iterator>
      typename Iterator::reference
      operator () (Iterator& it) const
      {
	return *it;
      }
    };

    struct iterator_init
    {
      template <typename Iterator>
      void operator() (Iterator& it) const { it.init(); }
    };

    struct iterator_next
    {
      template <typename Iterator>
      void operator() (Iterator& it) const { it.next(); }
    };

  };

  template <typename IteratorTuple>
  struct zip_iterator
    : iterator_base< zip_iterator<IteratorTuple>,
		     typename internal::tuple_meta_transform<IteratorTuple, internal::iterator_dereference>::type,
		     typename internal::tuple_meta_transform<IteratorTuple, internal::iterator_dereference>::type >
  {
    typedef typename internal::tuple_meta_transform<IteratorTuple, internal::iterator_dereference>::type value_type;
    typedef value_type reference;

    zip_iterator() {}

    zip_iterator(const IteratorTuple& tuple)
    : iterator_tuple_ (tuple)
    {
    }

    template <typename OtherIteratorTuple>
    zip_iterator(const zip_iterator<OtherIteratorTuple>& other,
		 typename std::enable_if< std::is_convertible<OtherIteratorTuple, IteratorTuple>::value >::type* = NULL)
      : iterator_tuple_ (other.iterator_tuple_)
    {
    }


    void init()
    {
      internal::tuple_for_each(iterator_tuple_, internal::iterator_init ());
    }

    void next()
    {
      internal::tuple_for_each(iterator_tuple_, internal::iterator_next ());
    }

    bool finished() const
    {
      return boost::get<0>(iterator_tuple_).finished();
    }

    reference dereference() const
    {
      return internal::tuple_transform(iterator_tuple_, internal::iterator_dereference ());
    }

  private:
    template <typename> friend struct zip_iterator;

    IteratorTuple iterator_tuple_;
  };

}

#endif // ! ZIP_ITERATOP_HPP
