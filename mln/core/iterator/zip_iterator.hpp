#ifndef ZIP_ITERATOP_HPP
# define ZIP_ITERATOP_HPP

# include <type_traits>
# include <tuple>
# include <mln/core/iterator/iterator_base.hpp>
# include <mln/core/internal/tuple_utility.hpp>

namespace mln
{

  template <typename IteratorTuple>
  struct zip_iterator;


  /********************/
  /* Implementation   */
  /********************/

  namespace internal
  {

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

  template <class... TTypes>
  struct zip_iterator< std::tuple<TTypes...> >
    : iterator_base< zip_iterator< std::tuple<TTypes...> >,
		     std::tuple< typename std::remove_reference<TTypes>::type::reference... >,
		     std::tuple< typename std::remove_reference<TTypes>::type::reference... > >
  {
    typedef std::tuple<TTypes...> IteratorTuple;
    typedef std::tuple< typename std::remove_reference<TTypes>::type::reference... > value_type;
    typedef value_type reference;

    zip_iterator() {}

    zip_iterator(const IteratorTuple& tuple)
    : m_iterator_tuple (tuple)
    {
    }

    template <typename OtherIteratorTuple>
    zip_iterator(const zip_iterator<OtherIteratorTuple>& other,
		 typename std::enable_if< std::is_convertible<OtherIteratorTuple, IteratorTuple>::value >::type* = NULL)
      : m_iterator_tuple (other.m_iterator_tuple)
    {
    }


    void init()
    {
      internal::tuple_for_each(m_iterator_tuple, internal::iterator_init ());
    }

    void next()
    {
      internal::tuple_for_each(m_iterator_tuple, internal::iterator_next ());
    }

    bool finished() const
    {
      return std::get<0>(m_iterator_tuple).finished();
    }

    reference dereference() const
    {
      return internal::tuple_transform(m_iterator_tuple, internal::iterator_dereference ());
    }

  private:
    template <typename> friend struct zip_iterator;

    IteratorTuple m_iterator_tuple;
  };

}

#endif // ! ZIP_ITERATOP_HPP
