#pragma once

#include <mln/core/colors.hpp>
#include <mln/core/vec_base.hpp>


namespace mln
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
    struct vec_base_traits<mln::colors::rgba_tag>
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

    using rgba8  = rgba<std::uint8_t>;
    using rgba16 = rgba<std::uint16_t>;
  } // namespace colors
} // namespace mln
