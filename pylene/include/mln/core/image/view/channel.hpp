#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/image/view/transform.hpp>
#include <mln/core/ops.hpp>
#include <mln/core/vec/vec_ops.hpp>


namespace mln::view
{
  template <typename InputImage>
  auto channel(InputImage ima, int k)
  {
    static_assert(is_a<InputImage, mln::details::Image>());

    return mln::view::transform(std::move(ima), dyn_getter{k});
  }

  template <unsigned k, typename InputImage>
  auto channel(InputImage ima)
  {
    static_assert(is_a<InputImage, mln::details::Image>());

    return mln::view::transform(std::move(ima), getter<k>{});
  }
} // namespace mln::view
