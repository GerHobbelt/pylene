#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/image/view/channel.hpp>


namespace mln::view
{
  template <typename InputImage>
  auto red(InputImage ima)
  {
    static_assert(is_a<InputImage, mln::details::Image>());

    return mln::view::channel<0>(std::move(ima));
  }

  template <typename InputImage>
  auto green(InputImage ima)
  {
    static_assert(is_a<InputImage, mln::details::Image>());

    return mln::view::channel<1>(std::move(ima));
  }

  template <typename InputImage>
  auto blue(InputImage ima)
  {
    static_assert(is_a<InputImage, mln::details::Image>());

    return mln::view::channel<2>(std::move(ima));
  }
} // namespace mln::view
