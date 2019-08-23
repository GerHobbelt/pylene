#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/rangev3/rows.hpp>
#include <mln/core/rangev3/view/zip.hpp>
#include <mln/core/trace.hpp>

#include <range/v3/algorithm/copy.hpp>

#include <algorithm>

/// \file

namespace mln
{


  /// \brief Copy a source image to an output image regardless domain.
  ///
  /// \ingroup Algorithms
  ///
  /// The output image must have a domain larger than the input image.
  /// This is equivalent to the following code.
  ///
  /// \code
  /// mln_iter(vin, input.values())
  /// mln_iter(vout, output.values())
  /// mln_forall(vin, vout)
  ///   *vout = *vin;
  /// \endcode
  ///
  /// \param[in] input Input Image
  /// \param[out] output Output Image
  /// \return The image where values have been copied in.
  ///
  /// \todo add specialization for raw images
  ///
  template <typename InputImage, typename OutputImage>
  [[deprecated]] OutputImage& copy(const Image<InputImage>& input, Image<OutputImage>& output);

  /// \overload
  /// \ingroup Algorithms
  template <typename InputImage, typename OutputImage>
  [[deprecated]] OutputImage&& copy(const Image<InputImage>& input, Image<OutputImage>&& output);


  namespace experimental
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
  } // namespace experimental

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  namespace impl
  {

    template <typename I, typename J>
    inline void copy(const I& input, J&& output)
    {
      mln_viter(vin, vout, input, output);

      mln_forall (vin, vout)
        *vout = (mln_value(I)) * vin;
    }
  } // namespace impl

  template <typename InputImage, typename OutputImage>
  OutputImage& copy(const Image<InputImage>& input, Image<OutputImage>& output)
  {
    static_assert(std::is_convertible<mln_value(InputImage), mln_value(OutputImage)>::value,
                  "The input image value type must be convertible to the output image value type");

    impl::copy(exact(input), exact(output));
    return exact(output);
  }

  template <typename InputImage, typename OutputImage>
  OutputImage&& copy(const Image<InputImage>& input, Image<OutputImage>&& output)
  {
    static_assert(std::is_convertible<mln_value(InputImage), mln_value(OutputImage)>::value,
                  "The input image value type must be convertible to the output image value type");

    impl::copy(exact(input), exact(output));
    return move_exact(output);
  }


  namespace experimental
  {
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
  } // namespace experimental
} // namespace mln
