#pragma once

#include <py/core/image2d.hpp>
#include <py/core/type_info.hpp>

#include <algorithm>
#include <limits>


namespace mln::py
{
  namespace impl
  {
    template <class T>
    image2d<float> stretch(const image2d<T>& src)
    {
    auto        res  = image2d<float>(src.width(), src.height());
    auto        span = src.values();
    const auto& vs   = src.get_value_set();
    std::transform(span.begin(), span.end(), res.values().begin(),
                   [&vs](auto val) -> float {
                     auto tmp = vs.max();
                     return vs.template cast<float>(val) / vs.template cast<float>(tmp);
                   });
    return res;
    }
  }

  namespace detail
  {
    template <typename T>
    struct apply_stretch_t
    {
      auto operator()(const image2d<>& src) { return impl::stretch(*src.cast_to<T>()); }
    };
  } // namespace detail
  image2d<float> stretch(const image2d<>& src) { return visit<detail::apply_stretch_t>(src.type().tid(), src); };

  image2d<> stretch_py(const image2d<>& src) { return stretch(src); };
} // namespace mln::py
