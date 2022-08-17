#pragma once

#include <ano/core/ops.hpp>
#include <ano/core/vec.hpp>

#include <cstdint>


namespace ano
{

  struct rgb_tag
  {
  };
  struct bgr_tag
  {
  };

  namespace internal
  {
    template <>
    struct vec_base_traits<rgb_tag>
    {
      static const bool is_additive             = true;
      static const bool is_additive_ext         = true;
      static const bool is_multiplicative       = false;
      static const bool is_multiplicative_ext   = true;
      static const bool is_less_than_comparable = false;
      static const bool is_equality_comparable  = true;
    };

    template <>
    struct vec_base_traits<bgr_tag>
    {
      static const bool is_additive             = true;
      static const bool is_additive_ext         = true;
      static const bool is_multiplicative       = false;
      static const bool is_multiplicative_ext   = true;
      static const bool is_less_than_comparable = false;
      static const bool is_equality_comparable  = true;
    };
  } // namespace internal

  template <typename T>
  using rgb = internal::vec_base<T, 3, rgb_tag>;

  using rgb8         = rgb<std::uint8_t>;
  using signed_rgb8  = rgb<std::int8_t>;
  using rgb16        = rgb<std::uint16_t>;
  using signed_rgb16 = rgb<std::int16_t>;

  template <typename T>
  using bgr = internal::vec_base<T, 3, bgr_tag>;

  using bgr8         = bgr<std::uint8_t>;
  using signed_bgr8  = bgr<std::int8_t>;
  using bgr16        = bgr<std::uint16_t>;
  using signed_bgr16 = bgr<std::int16_t>;

} // namespace ano
