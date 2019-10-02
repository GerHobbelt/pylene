#pragma once

#include <mln/morpho/experimental/rank_filter.hpp>

#include <ratio>


/// \file

namespace mln::morpho::experimental
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
  template <class InputImage, class SE, class BorderManager>
  image_concrete_t<std::remove_reference_t<InputImage>>
  median_filter(InputImage&& image, const mln::experimental::StructuringElement<SE>& se, BorderManager bm);

  template <class InputImage, class SE, class BorderManager, class OutputImage>
  void median_filter(InputImage&& image, const mln::experimental::StructuringElement<SE>& se, BorderManager bm,
                     OutputImage&& out);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <class InputImage, class SE, class BorderManager, class OutputImage>
  void median_filter(InputImage&& input, const mln::experimental::StructuringElement<SE>& se, BorderManager bm,
                     OutputImage&& out)
  {
    using R = std::ratio<1, 2>;
    rank_filter<R>(std::forward<InputImage>(input), se, bm, out);
  }


  template <class InputImage, class SE, class BorderManager>
  image_concrete_t<std::remove_reference_t<InputImage>>
  median_filter(InputImage&& image, const mln::experimental::StructuringElement<SE>& se, BorderManager bm)
  {
    using R = std::ratio<1, 2>;
    return rank_filter<R>(std::forward<InputImage>(image), se, bm);
  }

} // namespace mln
