#pragma once

#include <ano/core/algorithm/copy.hpp>


/// \file
/// \brief Header file for clone algorithm.

namespace ano
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
    static_assert(ano::is_a<InputImage, ano::details::Image>());

    image_concrete_t<InputImage> out = input.concretize();
    ano::copy(std::move(input), out);
    return out;
  }


  namespace parallel
  {
    template <class InputImage>
    image_concrete_t<InputImage> clone(InputImage input)
    {
      static_assert(ano::is_a<InputImage, ano::details::Image>());

      image_concrete_t<InputImage> out = input.concretize();
      ano::parallel::copy(std::move(input), out);
      return out;
    }
  } // namespace parallel

} // namespace ano
