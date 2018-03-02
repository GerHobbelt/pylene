#ifndef MLN_CORE_IMAGE_IMAGE_HPP
#warning "This should not be included as a standalone."
#include <mln/core/image/image.hpp>
#endif

#ifndef MLN_CORE_CH_VALUE_HPP
#define MLN_CORE_CH_VALUE_HPP

#include <mln/core/image_traits.hpp>

/// \file
/// \brief Meta-Function to transform images.

#define mln_ch_value(I, V) typename mln::image_ch_value<typename std::decay<I>::type, V>::type
#define mln_concrete(I) typename mln::image_concrete<typename std::decay<I>::type>::type

namespace mln
{

  template <typename Image>
  struct image_concrete;

  // If no specialization get concrete image from domain
  template <typename Image, typename V>
  struct image_ch_value;

}

#endif // !MLN_CORE_CH_VALUE_HPP
