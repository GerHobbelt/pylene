#pragma once

#include <ano/core/image/image.hpp>
#include <ano/core/image/view/channel.hpp>


namespace ano::view
{
  template <typename InputImage>
  auto red(InputImage ima)
  {
    static_assert(is_a<InputImage, ano::details::Image>());

    return ano::view::channel<0>(std::move(ima));
  }

  template <typename InputImage>
  auto green(InputImage ima)
  {
    static_assert(is_a<InputImage, ano::details::Image>());

    return ano::view::channel<1>(std::move(ima));
  }

  template <typename InputImage>
  auto blue(InputImage ima)
  {
    static_assert(is_a<InputImage, ano::details::Image>());

    return ano::view::channel<2>(std::move(ima));
  }
} // namespace ano::view
