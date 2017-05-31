#ifndef ZIP_ITERATOP_HPP
# define ZIP_ITERATOP_HPP

# include <type_traits>
# include <tuple>
# include <mln/core/assert.hpp>
# include <mln/core/iterator/iterator_base.hpp>
# include <mln/core/internal/tuple_utility.hpp>
# include <boost/mpl/fold.hpp>
# include <boost/mpl/list.hpp>
# include <boost/mpl/and.hpp>

namespace mln
{

  template <typename IteratorTuple>
  struct zip_iterator;


  /********************/
  /* Implementation   */
  /********************/


  template <class... TTypes>
  struct zip_iterator< std::tuple<TTypes...> >
    : iterator_base< zip_iterator< std::tuple<TTypes...> >,
		     std::tuple< typename std::remove_reference<TTypes>::type::reference... >,
		     std::tuple< typename std::remove_reference<TTypes>::type::reference... > >
  {
    typedef std::tuple<TTypes...> IteratorTuple;
    typedef std::tuple< typename std::remove_reference<TTypes>::type::reference... > value_type;
    typedef value_type reference;
    typedef std::integral_constant<
      bool,
      boost::mpl::fold<boost::mpl::list<typename TTypes::has_NL...>,
                       boost::mpl::true_,
                       boost::mpl::and_<boost::mpl::_1, boost::mpl::_2> >::type::value> has_NL;

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
      internal::tuple_for_each(m_iterator_tuple, [](auto& x) { x.init(); });
    }

    void next()
    {
      internal::tuple_for_each(m_iterator_tuple, [](auto& x) { x.next(); });
    }

    bool finished() const
    {
      return std::get<0>(m_iterator_tuple).finished();
    }

    void __inner_init()
    {
      internal::tuple_for_each(m_iterator_tuple, [](auto& x) { x.__inner_init(); });
    }

    void __outer_init()
    {
      internal::tuple_for_each(m_iterator_tuple, [](auto& x) { x.__outer_init(); });
    }

    void __inner_next()
    {
      internal::tuple_for_each(m_iterator_tuple, [](auto& x) { x.__inner_next(); });
    }

    void __outer_next()
    {
      internal::tuple_for_each(m_iterator_tuple, [](auto& x) { x.__outer_next(); });
    }


    bool __inner_finished() const
    {
      bool is_finished = false;
      internal::tuple_for_each(m_iterator_tuple, [&is_finished](const auto& x) { is_finished |= x.__inner_finished(); });
      return is_finished;
    }

    bool __outer_finished() const
    {
      bool finished = std::get<0>(m_iterator_tuple).__outer_finished();
      if (MLN_HAS_DEBUG && finished)
        internal::tuple_for_each(m_iterator_tuple, [](const auto& x) { mln_assertion(x.__outer_finished()); });
      return finished;
    }

    reference dereference() const
    {
      return internal::tuple_transform(m_iterator_tuple, [](auto& x) -> decltype(auto) { return x.dereference(); });
    }

    template <class dummy = bool>
    typename std::enable_if<has_NL::value, dummy>::type
    NL() const
    {
      return std::get<0>(m_iterator_tuple).NL();
    }

  private:
    template <typename> friend struct zip_iterator;

    IteratorTuple m_iterator_tuple;
  };

}

#endif // ! ZIP_ITERATOP_HPP
