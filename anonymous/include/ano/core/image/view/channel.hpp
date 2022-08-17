#pragma once

#include <ano/core/image/image.hpp>
#include <ano/core/image/view/transform.hpp>
#include <ano/core/ops.hpp>
#include <ano/core/vec/vec_ops.hpp>


namespace ano::view
{
  template <typename InputImage>
  auto channel(InputImage ima, int k)
  {
    static_assert(is_a<InputImage, ano::details::Image>());

    return ano::view::transform(std::move(ima), dyn_getter{k});
  }

  template <unsigned k, typename InputImage>
  auto channel(InputImage ima)
  {
    static_assert(is_a<InputImage, ano::details::Image>());

    return ano::view::transform(std::move(ima), getter<k>{});
  }
} // namespace ano::view
