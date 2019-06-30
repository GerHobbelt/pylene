#pragma once

#include <py/core/image2d.hpp>
#include <py/core/type_info.hpp>

namespace mln::py
{
  namespace impl
  {
    template <class T>
    image2d<uint8_t> invert(const image2d<T>& src)
    {
    auto        res  = image2d<uint8_t>(src.width(), src.height());
    auto        span = src.values();
    const auto& vs   = src.get_value_set();
    std::transform(span.begin(), span.end(), res.values().begin(),
                   [&vs](auto val) -> uint8_t {
                     auto tmp = vs.max();
                     auto new_val = vs.template cast<float>(val) / vs.template cast<float>(tmp);
                     return (1 - new_val) * 255;
                   });
    return res;
    }
  }

  image2d<uint8_t> invert(const image2d<>& src)
  {
    switch (src.type().tid())
    {
      case (Info::INT8_V):
        return impl::invert(*src.cast_to<int8_t>());
      case (Info::INT32_V):
        return impl::invert(*src.cast_to<int32_t>());
      default:
        return impl::invert(src);
    }
  };

  image2d<> invert_py(const image2d<>& src) { return invert(src); };
} // namespace mln::py