#ifndef KINTERPOLATE_HPP
# define KINTERPOLATE_HPP

# include <mln/core/image/image2d.hpp>

namespace mln
{

  /// \brief perform a median interpolation to make the iamge well-composed
  /// Note: the values are doubled to prevent rounding error
  template <typename U, typename V = decltype( std::declval<U>() * 2),
	    class Compare = std::less<U> >
  image2d<V>
  interpolate_median(const image2d<U>& ima, const V& = V(), const Compare& = Compare());


  /*******************************/
  /***    Implementation       ***/
  /*******************************/

  template <typename U, typename V, class Compare>
  image2d<V>
  interpolate_median(const image2d<U>& ima, const V&, const Compare& cmp)
  {
    image2d<V> out(2*ima.nrows()-1, 2*ima.ncols()-1);

    auto med4x2 = [&cmp] (const U& a, const U& b, const U& c, const U& d) {
      U min1, min2, max1, max2;
      std::tie(min1, max1) = std::minmax(a,b, cmp);
      std::tie(min2, max2) = std::minmax(c,d, cmp);
      return std::min(max1, max2, cmp) + std::max(min1, min2, cmp);
    };

    mln_foreach(const point2d& p, ima.domain())
      {
	U a = ima.at(p),
	  b = ima.at(p + point2d{0,1}),
	  c = ima.at(p + point2d{1,0}),
	  d = ima.at(p + point2d{1,1});

	point2d q = 2 * p;
	out.at(q) = 2 * ima.at(p);
	out.at(q + point2d{0,1}) = (a + b);
	out.at(q + point2d{1,0}) = (a + c);
	out.at(q + point2d{1,1}) = med4x2(a,b,c,d);
      }
    return out;
  }


}


#endif // ! KINTERPOLATE_HPP
