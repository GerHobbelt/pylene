#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/image/view/transform.hpp>

#include <utility>


namespace mln::view
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
    static_assert(is_a<InputImage, mln::details::Image>());

    return view::transform(std::move(ima), detail::cast_to<V>{});
  }
} // namespace mln::view
