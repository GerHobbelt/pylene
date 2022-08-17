#pragma once

#include <ano/core/extension/extension_traits.hpp>
#include <ano/core/image/image.hpp>
#include <ano/core/trace.hpp>


namespace ano
{
  namespace extension
  {

    template <typename I>
    const I& mirror(const Image<I>& ima);

    template <typename I>
    I&& mirror(Image<I>&& ima);

    /******************************************/
    /****          Implementation          ****/
    /******************************************/

    template <typename I>
    const I& mirror(const Image<I>& ima)
    {
      static_assert(image_has_extension<I>::value, "Image must have an extension.");
      static_assert(extension_traits<typename image_extension_type<I>::type>::support_fill::value,
                    "Image extension must support filling.");
      ano_entering("ano::extension::mirror");
      exact(ima).extension().mirror();
      ano_exiting();
      return exact(ima);
    }

    template <typename I>
    I&& mirror(Image<I>&& ima)
    {
      static_assert(image_has_extension<I>::value, "Image must have an extension.");
      static_assert(extension_traits<typename image_extension_type<I>::type>::support_fill::value,
                    "Image extension must support mirror.");

      ano_entering("ano::extension::mirror");
      exact(ima).extension().mirror();
      ano_exiting();
      return move_exact(ima);
    }

  } // namespace extension
} // namespace ano
