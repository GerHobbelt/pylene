#ifndef GRADIENT_HPP
# define GRADIENT_HPP

# include <mln/core/canvas/accfpfn.hpp>
# include <mln/core/value/value_traits.hpp>
# include <mln/core/neighborhood.hpp>


namespace mln
{

  template <typename T>
  struct Minmax
  {
    void init() {
      min = value_traits<T>::max();
      max = value_traits<T>::min();
    }

    void take(const T& v) {
      if (v < min)
	min = v;
      if (v > max)
	max = v;
    }


    T min, max;
  };

  struct rectangle2d : neighborhood_base< rectangle2d, box2d>
  {
    box2d dpoints;
  };



  template <typename T>
  image2d<T>
  gradient(const image2d<T>& ima, int size)
  {
    mln_precondition( size % 2 == 1 and size > 1 );
    int radius = (size - 1) / 2;

    rectangle2d win;
    win.dpoints = {{-radius, -radius}, {radius+1, radius+1}};
    return accfpfn(ima, win, true, Minmax<T> (), [] (const Minmax<T>& a) -> T {
	return a.max - a.min; });
  }


}

#endif // ! GRADIENT_HPP
