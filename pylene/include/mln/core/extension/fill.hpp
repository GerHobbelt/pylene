#pragma once

#include <mln/core/extension/extension_traits.hpp>
#include <mln/core/image/image.hpp>


namespace mln::extension
{


  template <typename I>
  I&& fill(Image<I>&& ima, mln_value(I) v);


  template <class InputImage>
  std::enable_if_t<mln::is_a<InputImage, mln::experimental::Image>::value ||
                   mln::is_a<InputImage, mln::Image>::value>
  fill(const InputImage& f, image_value_t<InputImage> v);

  template <class InputImage>
  bool try_fill(const InputImage& f, image_value_t<InputImage> v);

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <typename InputImage>
  std::enable_if_t<mln::is_a<InputImage, mln::experimental::Image>::value ||
                   mln::is_a<InputImage, mln::Image>::value>
  fill(const InputImage& ima, image_value_t<InputImage> v)
  {
    static_assert(image_has_extension<InputImage>::value, "Image must have an extension.");
    static_assert(extension_traits<typename image_extension_type<InputImage>::type>::support_fill::value,
                  "Image extension must support filling.");

    ima.extension().fill(v);
  }

  template <typename I>
  I&& fill(Image<I>&& ima, mln_value(I) v)
  {
    static_assert(image_has_extension<I>::value, "Image must have an extension.");
    static_assert(extension_traits<typename image_extension_type<I>::type>::support_fill::value,
                  "Image extension must support filling.");

    exact(ima).extension().fill(v);
    return move_exact(ima);
  }

  template <class InputImage>
  bool try_fill(const InputImage& f, image_value_t<InputImage> v)
  {
    static_assert(mln::is_a<InputImage, mln::experimental::Image>());

    using I = std::remove_reference_t<InputImage>;
    if constexpr (image_has_extension<I>::value)
      if constexpr (image_extension_t<I>::support_fill::value)
      {
        f.extension().fill(v);
        return true;
      }
    return false;
  }


} // namespace mln::extension
