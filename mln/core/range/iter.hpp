#ifndef MLN_CORE_RANGE_ITER_HPP
#define MLN_CORE_RANGE_ITER_HPP

#include <mln/core/iterator/stditerator.hpp>
#include <mln/core/range/range_traits.hpp>

namespace mln
{

  namespace rng
  {

    template <typename R>
    auto iter(R& range);

    template <typename R>
    auto iter(const R& range);

    template <typename R>
    auto riter(R& range);

    template <typename R>
    auto riter(const R& range);

    /*********************/
    /** Implementation  **/
    /*********************/

    namespace impl
    {
      // This is a real MLN range
      template <typename R>
      auto iter(R& range, std::true_type /* is_an_mln_range */, std::false_type /* is_an_mln_iterator */)
      {
        return range.iter();
      }

      /// This is a pseudo-range (MLN Iterator)
      template <typename R>
      auto iter(R& range, std::true_type /* is_an_mln_range */, std::true_type /* is_an_mln_iterator */)
      {
        return range;
      }

      // This is a STL range
      template <typename R>
      auto iter(R& range, std::false_type /* is_an_mln_range */, std::false_type /* is_an_mln_iterator */)
      {
        typename range_iterator<R>::type x(range.begin(), range.end());
        return x;
      }

      // This is a real MLN range
      template <typename R>
      auto riter(R& range, std::true_type /* is_an_mln_range */, std::false_type /* is_an_mln_iterator */)
      {
        return range.riter();
      }

      /// This is a pseudo-range (MLN Iterator)
      template <typename R>
      auto riter(R& range, std::true_type /* is_an_mln_range */, std::true_type /* is_an_mln_iterator */)
      {
        return range.riter(); // i.e itself
      }

      // This is a STL range
      template <typename R>
      auto riter(R& range, std::false_type /* is_an_mln_range */, std::false_type /* is_an_mln_iterator */)
      {
        typename range_reverse_iterator<R>::type x(range.rbegin(), range.rend());
        return x;
      }
    }

    template <typename R>
    auto iter(R& range)
    {
      return impl::iter(range, is_mln_range<R>(), is_a<R, Iterator>());
    }

    template <typename R>
    auto iter(const R& range)
    {
      return impl::iter(range, is_mln_range<R>(), is_a<R, Iterator>());
    }

    template <typename R>
    auto riter(R& range)
    {
      return impl::riter(range, is_mln_range<R>(), is_a<R, Iterator>());
    }

    template <typename R>
    auto riter(const R& range)
    {
      return impl::riter(range, is_mln_range<R>(), is_a<R, Iterator>());
    }
  }
}

#endif // ! MLN_CORE_RANGE_ITER_HPP
