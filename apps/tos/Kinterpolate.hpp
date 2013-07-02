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


  /// \brief perform a interpolation by setting the mean on
  /// on 0-1 face from 2-faces.
  /// \param ima Original image
  /// \return An image twice as big as \p ima with mean on 0 abd 1 faces. 
  template <typename T>
  image2d<T>
  interpolate_k1(const image2d<T>& ima);



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


  template <typename T>
  image2d<T>
  interpolate_k1(const image2d<T>& ima)
  {
    image2d<T> out(2*ima.nrows()-1, 2*ima.ncols()-1);
    typedef point2d P;
    mln_foreach(point2d p, ima.domain())
      {
	T a = ima.at(p),
	  b = ima.at(p + P{0,1}),
	  c = ima.at(p + P{1,0}),
	  d = ima.at(p + P{1,1});

	point2d q = 2 * p;
	out.at(q) = ima.at(p);
	out.at(q + P{0,1}) = (a + b) / 2;
	out.at(q + P{1,0}) = (a + c) / 2;
	out.at(q + P{1,1}) = (a + b + c + d) / 4;
      }

    return out;
  }


}


#endif // ! KINTERPOLATE_HPP
