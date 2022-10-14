#pragma once

#include <mln/core/value/value_traits.hpp>

#include <mln/core/vec_base.hpp>
#include <mln/core/vec/compare.hpp>
#include <mln/core/vec/vec_maths_ops.hpp>
#include <mln/core/vec/vec_ops.hpp>



namespace mln
{

  template <typename T, unsigned dim>
  using vec = internal::vec_base<T, dim, generic_vector_tag>;

  using vec1ub = vec<unsigned char, 1>;
  using vec2ub = vec<unsigned char, 2>;
  using vec3ub = vec<unsigned char, 3>;
  using vec1b = vec<char, 1>;
  using vec2b = vec<char, 2>;
  using vec3b = vec<char, 3>;
  using vec1us = vec<unsigned short, 1>;
  using vec2us = vec<unsigned short, 2>;
  using vec3us = vec<unsigned short, 3>;
  using vec1s = vec<short, 1>;
  using vec2s = vec<short, 2>;
  using vec3s = vec<short, 3>;
  using vec1u = vec<unsigned, 1>;
  using vec2u = vec<unsigned, 2>;
  using vec3u = vec<unsigned, 3>;
  using vec1i = vec<int, 1>;
  using vec2i = vec<int, 2>;
  using vec3i = vec<int, 3>;
  using vec1f = vec<float, 1>;
  using vec2f = vec<float, 2>;
  using vec3f = vec<float, 3>;
  using vec1d = vec<double, 1>;
  using vec2d = vec<double, 2>;
  using vec3d = vec<double, 3>;
} // namespace mln
