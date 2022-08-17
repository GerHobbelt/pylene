#pragma once

#include <ano/core/colors.hpp>
#include <ano/core/vec_base.hpp>


namespace ano
{

  namespace colors
  {
    struct rgba_tag
    {
    };
  } // namespace colors

  namespace internal
  {
    template <>
    struct vec_base_traits<ano::colors::rgba_tag>
    {
      static const bool is_additive             = true;
      static const bool is_additive_ext         = true;
      static const bool is_multiplicative       = false;
      static const bool is_multiplicative_ext   = true;
      static const bool is_less_than_comparable = false;
      static const bool is_equality_comparable  = true;
    };
  } // namespace internal

  namespace colors
  {
    template <typename T>
    using rgba = internal::vec_base<T, 4, rgba_tag>;

    typedef rgba<std::uint8_t>  rgba8;
    typedef rgba<std::uint16_t> rgba16;
  } // namespace colors
} // namespace ano
