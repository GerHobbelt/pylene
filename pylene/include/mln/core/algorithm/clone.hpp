#pragma once

#include <mln/core/algorithm/copy.hpp>


/// \file
/// \brief Header file for clone algorithm.

namespace mln
{

  /// \ingroup Algorithms
  /// \brief Make and return a deep copy of an image.
  ///
  /// If the image is not already concrete (e.g. a morpher with no
  /// proper memory), the image is concretized.
  ///
  /// \tparam I The input image type
  /// \param ima The image to clone.
  template <class InputImage>
  image_concrete_t<InputImage> clone(InputImage input);

  /******************/
  /* Implem         */
  /******************/

  template <class InputImage>
  image_concrete_t<InputImage> clone(InputImage input)
  {
    static_assert(mln::is_a<InputImage, mln::details::Image>());

    image_concrete_t<InputImage> out = input.concretize();
    mln::copy(std::move(input), out);
    return out;
  }


  namespace parallel
  {
    template <class InputImage>
    image_concrete_t<InputImage> clone(InputImage input)
    {
      static_assert(mln::is_a<InputImage, mln::details::Image>());

      image_concrete_t<InputImage> out = input.concretize();
      mln::parallel::copy(std::move(input), out);
      return out;
    }
  } // namespace parallel

} // namespace mln
