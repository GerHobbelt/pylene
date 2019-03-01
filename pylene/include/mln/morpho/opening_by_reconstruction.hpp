#pragma once

#include <mln/morpho/closing_by_reconstruction.hpp>


namespace mln
{

  namespace morpho
  {

    template <class I, class J, class N>
    mln_concrete(I) opening_by_reconstruction(const Image<I>& f, const Image<J>& markers, const Neighborhood<N>& nbh);

    /*************************/
    /** Implementation     ***/
    /*************************/

    template <class I, class J, class N>
    mln_concrete(I) opening_by_reconstruction(const Image<I>& f, const Image<J>& markers, const Neighborhood<N>& nbh)
    {
      static_assert(std::is_convertible<mln_value(J), mln_value(I)>::value,
                    "Marker image value type must be convertible to f's value type.");

      static_assert(std::is_same<mln_point(J), mln_point(I)>::value,
                    "Images f and marker must have the same point type.");

      mln_entering("mln::morpho::opening_by_reconstruction");
      return closing_by_reconstruction(f, markers, nbh, std::greater<mln_value(I)>());
    }
  } // namespace morpho
} // namespace mln
