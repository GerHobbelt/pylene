#ifndef MLN_MORPHO_TOS_IMMERSE_HPP
# define MLN_MORPHO_TOS_IMMERSE_HPP

# include <mln/core/image/image.hpp>
# include <mln/morpho/tos/irange.hpp>

namespace mln
{

  namespace morpho
  {

    namespace tos
    {

      namespace internal
      {

	template <typename I, class Compare>
	mln_ch_value(I, irange<mln_value(I)> )
	  immerse(const Image<I>& ima, Compare cmp)
	{
	  static_assert(std::is_same<typename I::domain_type, box2d>::value,
			"Only box2d handled");

	  typedef mln_value(I) V;
	  typedef irange<V>    R;

	  box2d d = ima.domain();
	  d.pmax = d.pmax * 2 - 1;

	  image2d<R> out(d);

	  copy(ima, out | grid2d(d.pmin, d.pmax, 

	}




      }

    }

  }

}

#endif // ! MLN_MORPHO_TOS_IMMERSE_HPP
