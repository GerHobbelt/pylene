#pragma once

#include <py/core/image2d.hpp>

#include <algorithm>
#include <limits>


namespace mln::py
{
  struct stretch_t
  {
    image2d<void> operator()(const image2d<>& src) const;

    template <class T>
    image2d<float> operator()(const image2d<T>& src) const
    {
      auto res  = image2d<float>(src.width(), src.height());
      auto span = src.values();
      std::transform(span.begin(), span.end(), res.values().begin(),
                     [](T val) -> float { return static_cast<float>(val) / std::numeric_limits<T>::max(); });
      return res;
    }
  };

  inline constexpr const stretch_t stretch{};

} // namespace mln::py