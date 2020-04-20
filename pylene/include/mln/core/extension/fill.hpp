#pragma once

#include <mln/core/private/traits/extension.hpp>
#include <mln/core/image/image.hpp>


namespace mln::extension
{
  template <class InputImage>
  void fill(const InputImage& f, image_value_t<InputImage> v);

  template <class InputImage>
  bool try_fill(const InputImage& f, image_value_t<InputImage> v);

  /******************************************/
  /****          Implementation          ****/
  /******************************************/

  template <typename InputImage>
  void fill(const InputImage& ima, image_value_t<InputImage> v)
  {
    static_assert(mln::is_a<InputImage, mln::details::Image>::value);
    static_assert(image_has_extension_v<InputImage>, "Image must have an extension.");
    static_assert(image_extension_t<InputImage>::support_fill::value, "Image extension must support filling.");

    ima.extension().fill(v);
  }

  template <class InputImage>
  bool try_fill(const InputImage& f, image_value_t<InputImage> v)
  {
    static_assert(mln::is_a<InputImage, mln::details::Image>());

    using I = std::remove_reference_t<InputImage>;
    if constexpr (image_has_extension_v<I>)
      if constexpr (image_extension_t<I>::support_fill::value)
      {
        f.extension().fill(v);
        return true;
      }
    return false;
  }


} // namespace mln::extension
