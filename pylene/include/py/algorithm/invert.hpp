#pragma once

#include <py/core/image2d.hpp>
#include <py/core/type_info.hpp>

namespace mln::py
{
  template <class T>
  image2d<uint8_t> invert(const image2d<T>& src)
  {
    auto        res  = image2d<uint8_t>(src.width(), src.height());
    auto        span = src.values();
    const auto& vs   = src.get_value_set();
    std::transform(span.begin(), span.end(), res.values().begin(),
                   [&vs](auto val) -> uint8_t {
                     return 255 - val;
                   });
    return res;
  }

  namespace detail
  {
    template <typename T>
    struct apply_invert_t
    {
      auto operator()(const image2d<>& src) { return invert(*src.cast_to<T>()); }
    };
  } // namespace detail
  image2d<uint8_t> invert(const image2d<>& src) { return visit<detail::apply_invert_t>(src.type().tid(), src); };

  image2d<> invert_py(const image2d<>& src) { return invert(src); };
} // namespace mln::py