#pragma once

#include <mln/core/vec.hpp>


namespace mln
{
  struct point_tag
  {
  };

  namespace internal
  {

    template <>
    struct vec_base_traits<point_tag>
    {
      static const bool is_additive             = true;
      static const bool is_additive_ext         = true;
      static const bool is_multiplicative       = false;
      static const bool is_multiplicative_ext   = true;
      static const bool is_less_than_comparable = true;
      static const bool is_equality_comparable  = true;
    };
  } // namespace internal

  template <typename T, unsigned dim>
  using point = internal::vec_base<T, dim, point_tag>;

  typedef point<short, 1> point1d;
  typedef point<short, 2> point2d;
  typedef point<short, 3> point3d;
  typedef point<float, 1> point1df;
  typedef point<float, 2> point2df;
  typedef point<float, 3> point3df;
} // namespace mln
