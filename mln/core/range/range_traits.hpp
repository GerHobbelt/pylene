#ifndef RANGE_TRAITS_HPP
# define RANGE_TRAITS_HPP

# include <type_traits>
# include <mln/core/concept/iterator.hpp>
# include <mln/core/iterator/stditerator.hpp>

namespace mln
{

  template <typename Range>
  struct range_traits;
  //{
  // bidirectional true_type/false_type
  //};


  template <typename Range>
  struct is_mln_range;

  template <typename Range, bool mln_range = is_mln_range<Range>::value>
  struct range_iterator;

  template <typename Range, bool mln_range = is_mln_range<Range>::value>
  struct range_const_iterator;

  template <typename Range>
  struct range_value;

  template <typename Range>
  struct range_reference;

  /**************************/
  /** Implementation       **/
  /**************************/

  namespace impl
  {
    template <typename R, typename dummy = void>
    struct has_iter_member : std::false_type
    {
    };

    template <typename R>
    struct has_iter_member<R, decltype(std::declval<R>().iter(), (void)0)>
      : std::true_type
    {
    };
  }

  template <typename R>
  struct is_mln_range :
    mln::is_a<typename R::iterator, Iterator>
  {
  };

  template <typename R>
  struct range_iterator<R, true>
  {
    typedef typename R::iterator			type;
  };

  template <typename R>
  struct range_iterator<R, false>
  {
    typedef stditerator<typename R::iterator>		type;
  };

  template <typename R>
  struct range_iterator<const R, true>
  {
    typedef typename R::const_iterator			type;
  };

  template <typename R>
  struct range_iterator<const R, false>
  {
    typedef stditerator<typename R::const_iterator>	type;
  };

  template <typename R>
  struct range_const_iterator<R, true>
  {
    typedef typename R::const_iterator			type;
  };

  template <typename R>
  struct range_const_iterator<R, false>
  {
    typedef stditerator<typename R::const_iterator>	type;
  };

  template <typename R>
  struct range_value
  {
    typedef typename range_iterator<R>::type::value_type type;
  };

  template <typename R>
  struct range_reference
  {
    typedef typename range_iterator<R>::type::reference	type;
  };

}

#endif // ! RANGE_TRAITS_HPP
