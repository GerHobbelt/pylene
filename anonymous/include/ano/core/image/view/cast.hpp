#pragma once

#include <ano/core/image/image.hpp>
#include <ano/core/image/view/transform.hpp>

#include <utility>


namespace ano::view
{
  namespace detail
  {
    template <typename V>
    struct cast_to
    {
      template <typename T>
      V operator()(const T& v) const
      {
        return static_cast<V>(v);
      }
    };
  } // namespace detail

  template <class V, class InputImage>
  auto cast(InputImage ima)
  {
    static_assert(is_a<InputImage, ano::details::Image>());

    return view::transform(std::move(ima), detail::cast_to<V>{});
  }
} // namespace ano::view
