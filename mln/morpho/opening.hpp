#ifndef MLN_MORPHO_OPENING_HPP
# define MLN_MORPHO_OPENING_HPP

# include <mln/morpho/dilate.hpp>
# include <mln/morpho/erode.hpp>

namespace mln
{

  namespace morpho
  {

    template <class I, class SE, class Compare = std::less<mln_value(I)> >
    mln_concrete(I)
    opening(const Image<I>& ima,
            const StructuringElement<SE>& se,
            Compare cmp = Compare ());



    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    template <class I, class SE, class Compare>
    mln_concrete(I)
    opening(const Image<I>& ima,
            const StructuringElement<SE>& se,
            Compare cmp)
    {
      mln_entering("mln::morpho::opening");

      mln_concrete(I) f = dilate(erode(ima, se, cmp), se, cmp);

      mln_exiting();
      return f;
    }


  }

}

#endif // ! MLN_MORPHO_OPENING_HPP
