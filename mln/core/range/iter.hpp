#ifndef MLN_CORE_RANGE_ITER_HPP
# define MLN_CORE_RANGE_ITER_HPP

# include <mln/core/range/range_traits.hpp>
# include <mln/core/iterator/stditerator.hpp>

namespace mln
{

  namespace rng
  {

    template <typename R>
    typename range_iterator<R>::type
    iter(R& range);

    /*********************/
    /** Implementation  **/
    /*********************/

    namespace impl
    {
      template <typename R>
      typename range_iterator<R>::type
      iter(R& range, typename std::enable_if<
	     is_mln_range<R>::value>::type* = NULL)
      {
	return range.iter();
      }

      template <typename R>
      typename range_iterator<R>::type
      iter(R& range, typename std::enable_if<
	     !is_mln_range<R>::value>::type* = NULL)
      {
	typename range_iterator<R>::type x(range.begin(), range.end());
	return x;
      }
    }


    template <typename R>
    typename range_iterator<R>::type
    iter(R& range)
    {
      return impl::iter(range);
    }

    template <typename R>
    typename range_const_iterator<R>::type
    iter(const R& range)
    {
      return impl::iter(range);
    }


  }

}

#endif // ! MLN_CORE_RANGE_ITER_HPP
