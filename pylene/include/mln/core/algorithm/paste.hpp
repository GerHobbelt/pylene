#pragma once
#include <mln/core/concept/new/images.hpp>
#include <mln/core/rangev3/rows.hpp>

namespace mln
{

  /// \brief \p paste copy each pixel from the input image to the output image.
  ///
  /// \ingroup Algorithms
  ///
  /// Each pixel of the input image is paste to the output image. It implies
  /// that \p input's domain must be included in \p output's domain.

  /// This is equivalent to the following code:
  ///
  /// \code
  /// mln_foreach (auto px, input.pixels())
  ///    output(px.point()) = px.val()
  /// \endcode
  ///
  /// \param[in] input The input image.
  /// \param[out] output The output image
  ///
  /// \return The output image.
  ///
  /// \pre `output.domain()` includes `input.domain()`
  /// \tparam InputImage is a model of the Forward Image.
  /// \tparam OutputImage is a model of the Writable Point-Accessible Image
  template <class InputImage, class OutputImage>
  void paste(InputImage src, OutputImage dest);

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <class InputImage, class OutputImage>
  void paste(InputImage src, OutputImage dest)
  {
    // FIXME: Add a precondition about the domain inclusion
    // FIXME: check OutputImage is accessible
    static_assert(mln::is_a<InputImage, experimental::Image>());
    static_assert(mln::is_a<OutputImage, experimental::Image>());
    static_assert(std::is_convertible_v<image_value_t<InputImage>, image_value_t<OutputImage>>);

    auto&& pixels = src.new_pixels();
    for (auto row : ranges::rows(pixels))
      for (auto px : row)
        dest(px.point()) = px.val();
  }

} // namespace mln
