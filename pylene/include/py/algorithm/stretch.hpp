#pragma once

#include <py/core/image2d.hpp>

#include <algorithm>
#include <limits>


namespace mln::py
{
    template <class T>
    image2d<float> stretch(const image2d<T>& src)
    {
      auto res  = image2d<float>(src.width(), src.height());
      auto span = src.values();
      const auto &vs = src.get_value_set();
      std::transform(span.begin(), span.end(), res.values().begin(),
              [&vs](auto val) -> float { return vs.normalize(val); });
      return res;
    }
} // namespace mln::py
