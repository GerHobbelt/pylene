#ifndef MLN_CORE_NEIGHBORHOOD_NEIGHBORHOOD_BASE_HPP
#define MLN_CORE_NEIGHBORHOOD_NEIGHBORHOOD_BASE_HPP

#include <mln/core/concept/iterator.hpp>
#include <mln/core/concept/pixel.hpp>
#include <mln/core/iterator/iterator_traits.hpp>
#include <mln/core/neighborhood/neighborhood.hpp>

namespace mln
{

  template <class Nbh, class tag>
  struct neighborhood_base : Neighborhood<Nbh>
  {
    typedef tag category;
    typedef std::false_type is_incremental;

  private:
    /// \brief Overload if x is a point lvalue
    template <class X>
    auto dispatch(X& x, std::false_type __is_pixel, std::false_type __is_iterator) const
    {
      (void)__is_pixel;
      (void)__is_iterator;
      return mln::exact(this)->__bind_point(x);
    }

    /// \brief Overload if x is a point rvalue
    template <class X>
    auto dispatch(X&& x, std::false_type __is_pixel, std::false_type __is_iterator) const
    {
      (void)__is_pixel;
      (void)__is_iterator;
      return mln::exact(this)->__process_point(x);
    }

    /// \brief Overload if x is a pixel lvalue
    template <class X>
    auto dispatch(X& x, std::true_type __is_pixel, std::false_type __is_iterator) const
    {
      (void)__is_pixel;
      (void)__is_iterator;
      return mln::exact(this)->__bind_pixel(x);
    }

    /// \brief Overload if x is a pixel lvalue
    template <class X>
    auto dispatch(X& x, std::false_type __is_pixel, std::true_type __is_iterator) const
    {
      (void)__is_pixel;
      (void)__is_iterator;
      using V = typename iterator_traits<X>::value_type;
      return dispatch_iterator(x, is_a<V, Pixel>());
    }

    /// \brief Overload if x is a point iterator
    template <typename X>
    auto dispatch_iterator(X& x, std::false_type __is_pixel_iterator) const
    {
      (void)__is_pixel_iterator;
      return mln::exact(this)->__bind_point_iterator(x);
    }

    /// \brief Overload if x is a pixel iterator
    template <typename X>
    auto dispatch_iterator(X& x, std::true_type __is_pixel_iterator) const
    {
      (void)__is_pixel_iterator;
      return mln::exact(this)->__bind_pixel_iterator(x);
    }

  public:
    template <class X>
    auto operator()(X&& x) const
    {
      using T = std::remove_reference_t<X>;
      return dispatch(std::forward<X>(x), is_a<T, Pixel>(), is_a<T, Iterator>());
    }
  };
}

#endif // ! MLN_CORE_NEIGHBORHOOD_NEIGHBORHOOD_BASE_HPP
