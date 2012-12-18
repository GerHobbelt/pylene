#ifndef RANGE_TRAITS_HPP
# define RANGE_TRAITS_HPP

namespace mln
{

  template <typename Range>
  struct range_traits;
  //{
  // bidirectional true_type/false_type
  //};

  template <typename Range>
  struct range_iterator
  {
    typedef typename Range::iterator type;
  };

  template <typename Range>
  struct range_iterator<const Range>
  {
    typedef typename Range::const_iterator type;
  };

  template <typename Range>
  struct range_const_iterator
  {
    typedef typename Range::const_iterator type;
  };

  template <typename Range>
  struct range_value
  {
    typedef typename range_iterator<Range>::type::value_type type;
  };


}

#endif // ! RANGE_TRAITS_HPP
