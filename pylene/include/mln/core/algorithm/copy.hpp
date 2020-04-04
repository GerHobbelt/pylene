#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/rangev3/rows.hpp>
#include <mln/core/rangev3/view/zip.hpp>
#include <mln/core/trace.hpp>

#include <range/v3/algorithm/copy.hpp>

#include <algorithm>

/// \file

namespace mln::experimental
{

  /// \brief Copy the values of a source image to a destination image regardless their domain.
  ///
  /// \ingroup Algorithms
  ///
  /// The input and output image must have domain of the same size.
  /// This is equivalent to the following code.
  ///
  /// \code
  /// mln_foreach((auto&& [vin, vout]), ranges::zip(input.values(), output.values()))
  ///   *vout = *vin;
  /// \endcode
  ///
  /// \param[in] src Input Image
  /// \param[out] dest Output Image
  /// \return The image where values have been copied in.
  ///
  ///
  template <class InputImage, class OutputImage>
  void copy(InputImage src, OutputImage dest);


  /******************************************/
  /****          Implementation          ****/
  /******************************************/


  template <class InputImage, class OutputImage>
  void copy(InputImage input, OutputImage output)
  {
    mln_entering("mln::copy");

    // FIXME: Add a precondition about the size of the domain ::ranges::size
    static_assert(mln::is_a<InputImage, Image>());
    static_assert(mln::is_a<OutputImage, Image>());
    static_assert(std::is_convertible_v<image_value_t<InputImage>, image_value_t<OutputImage>>);

    auto&& ivals = input.new_values();
    auto&& ovals = output.new_values();

    for (auto [r1, r2] : ranges::view::zip(ranges::rows(ivals), ranges::rows(ovals)))
      ::ranges::copy(r1, ::ranges::begin(r2));
  }

} // namespace mln::experimental
