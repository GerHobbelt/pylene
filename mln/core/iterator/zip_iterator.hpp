#ifndef ZIP_ITERATOP_HPP
# define ZIP_ITERATOP_HPP

# include <type_traits>
# include <tuple>
# include <mln/core/assert.hpp>
# include <mln/core/iterator/iterator_base.hpp>
# include <mln/core/internal/tuple_utility.hpp>

namespace mln
{

  template <typename IteratorTuple>
  struct zip_iterator;

  template <class... TTypes>
  auto make_zip_iterator(TTypes&&... iterators)
  {
    return zip_iterator<std::tuple<TTypes&&...>>(std::forward_as_tuple(iterators...));
  }


  /********************/
  /* Implementation   */
  /********************/

  namespace details
  {

    template <bool IsMultiDimensional, class... IteratorTypes>
    struct zip_iterator_base;

    template <class... IteratorTypes>
    struct zip_iterator_base<false, IteratorTypes...>
      : iterator_base< zip_iterator< std::tuple<IteratorTypes...> >,
		     std::tuple< typename std::remove_reference_t<IteratorTypes>::reference... >,
		     std::tuple< typename std::remove_reference_t<IteratorTypes>::reference... > >
    {
      using IteratorTuple = std::tuple<IteratorTypes...>;
      using value_type = std::tuple<typename std::remove_reference_t<IteratorTypes>::reference...>;
      using reference = value_type;
      using has_NL = std::conjunction<typename std::remove_reference_t<IteratorTypes>::has_NL...>;
      using is_multidimensional = std::false_type;

      zip_iterator_base() = default;
      zip_iterator_base(const IteratorTuple& tuple) : m_iterator_tuple(tuple) {}

      template <class OtherIteratorTuple>
      zip_iterator_base(const zip_iterator<OtherIteratorTuple>& other)
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
        bool finished = std::get<0>(m_iterator_tuple).finished();
        if (MLN_HAS_DEBUG && finished)
        {
          internal::tuple_for_each(m_iterator_tuple, [](const auto& x) {
            (void)x;
            mln_assertion(x.finished() && "One of the iterator is not finished.");
          });
        }
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

    protected:
      template <typename> friend struct zip_iterator;

      IteratorTuple m_iterator_tuple;
    };


    template <class... Iterators>
    struct zip_iterator_base<true, Iterators...> : zip_iterator_base<false, Iterators...>
    {
      using is_multidimensional = std::true_type;

      // Inherit constructors
      using zip_iterator_base<false, Iterators...>::zip_iterator_base;

      void __inner_init()
      {
        internal::tuple_for_each(this->m_iterator_tuple, [](auto& x) { x.__inner_init(); });
      }

      void __outer_init()
      {
        internal::tuple_for_each(this->m_iterator_tuple, [](auto& x) { x.__outer_init(); });
      }

      void __inner_next()
      {
        internal::tuple_for_each(this->m_iterator_tuple, [](auto& x) { x.__inner_next(); });
      }

      void __outer_next()
      {
        internal::tuple_for_each(this->m_iterator_tuple, [](auto& x) { x.__outer_next(); });
      }


      bool __inner_finished() const
      {
        bool finished = std::get<0>(this->m_iterator_tuple).__inner_finished();
        if (MLN_HAS_DEBUG && finished)
          internal::tuple_for_each(this->m_iterator_tuple, [](const auto& x) { (void)x; mln_assertion(x.__inner_finished()); });
        return finished;
      }

      bool __outer_finished() const
      {
        bool finished = std::get<0>(this->m_iterator_tuple).__outer_finished();
        if (MLN_HAS_DEBUG && finished)
          internal::tuple_for_each(this->m_iterator_tuple, [](const auto& x) { (void)x; mln_assertion(x.__outer_finished()); });
        return finished;
      }
    };
  }

  template <class... TTypes>
  struct zip_iterator<std::tuple<TTypes...>>
    : details::zip_iterator_base<std::conjunction<typename std::remove_reference_t<TTypes>::is_multidimensional...>::value, TTypes...>
  {
    using is_multidimensional = typename std::conjunction<typename std::remove_reference_t<TTypes>::is_multidimensional...>::type;
    using details::zip_iterator_base<is_multidimensional::value, TTypes...>::zip_iterator_base;
  };

}

#endif // ! ZIP_ITERATOP_HPP
