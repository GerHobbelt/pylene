#pragma once

#include <mln/morpho/rank_filter.hpp>

#include <ratio>


/// \file

namespace mln
{

  namespace morpho
  {

    /// \ingroup morpho
    /// \brief Median filter
    ///
    /// A median filter is non-linerar filter using the value ordering to
    /// compute the filtered value. It computes:
    /// \f[
    /// g(x) = med(\{ f(y) \in \mathcal{B}_x) \})
    /// \f]
    /// where \p med returns the median value of the set of pixels of the
    /// structuring element 𝑩 centered in 𝑥.
    ///
    /// \note The algorithm internally uses the function rank_filter with
    /// the ratio set to `std::ratio<1,2>`.
    ///
    /// \param ima Input image 𝑓
    /// \param se  Structuring element
    /// \param out (optional) Output image 𝑔 so store the result.
    template <class I, class SE, class OutputImage>
    OutputImage& median_filter(const Image<I>& ima, const StructuringElement<SE>& se, Image<OutputImage>& out);

    /// \ingroup morpho
    /// \overload  OutputImage& median_filter(const Image<I>&, const StructuringElement<SE>&, Image<OutputImage>&);
    template <class I, class SE>
    mln_concrete(I) median_filter(const Image<I>& ima, const StructuringElement<SE>& se);

    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    template <class I, class SE, class OutputImage>
    OutputImage& median_filter(const Image<I>& ima, const StructuringElement<SE>& se, Image<OutputImage>& out)
    {
      return rank_filter<std::ratio<1, 2>>(ima, se, out);
    }

    template <class I, class SE>
    mln_concrete(I) median_filter(const Image<I>& ima, const StructuringElement<SE>& se)
    {
      return rank_filter<std::ratio<1, 2>>(ima, se);
    }

  } // namespace morpho

} // namespace mln
