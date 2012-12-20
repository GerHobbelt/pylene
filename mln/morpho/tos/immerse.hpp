#ifndef MLN_MORPHO_TOS_IMMERSE_HPP
# define MLN_MORPHO_TOS_IMMERSE_HPP

# include <mln/core/image/image.hpp>
# include <mln/core/image/image2d.hpp>
# include <mln/core/image/sub_image.hpp>
# include <mln/morpho/tos/irange.hpp>
# include <mln/io/imprint.hpp>

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
	  immerse(const Image<I>& ima_, Compare cmp)
	{
	  static_assert(std::is_same<typename I::domain_type, box2d>::value,
			"Only box2d handled");

	  typedef mln_value(I) V;
	  typedef irange<V>    R;
	  typedef point2d      P;
	  const I& ima = exact(ima_);

	  image2d<R> out(2*ima.nrows()-1, 2*ima.ncols()-1);

	  mln_foreach(point2d p, ima.domain())
	    {
	      V a = ima.at(p),
		b = ima.at(p + P{0,1}),
		c = ima.at(p + P{1,0}),
		d = ima.at(p + P{1,1});

	      V min1 = std::min(a,b), min2 = std::min(a,c);
	      V max1 = std::max(a,b), max2 = std::max(a,c);
	      V min3 = std::min(d, std::min(c, min1));
	      V max3 = std::max(d, std::max(c, max1));

	      point2d q = 2 * p;
	      out.at(q) = ima.at(p);
	      out.at(q + P{0,1}) = R{min1, max1};
	      out.at(q + P{1,0}) = R{min2, max2};
	      out.at(q + P{1,1}) = R{min3, max3};
	    }

	  return out;
	}

      }

    }

  }

}

#endif // ! MLN_MORPHO_TOS_IMMERSE_HPP
