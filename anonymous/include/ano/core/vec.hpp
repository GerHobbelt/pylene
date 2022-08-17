#pragma once

#include <ano/core/value/value_traits.hpp>

#include <ano/core/vec/compare.hpp>
#include <ano/core/vec/vec_maths_ops.hpp>
#include <ano/core/vec/vec_ops.hpp>
#include <ano/core/vec_base.hpp>


namespace ano
{

  template <typename T, unsigned dim>
  using vec = internal::vec_base<T, dim, generic_vector_tag>;

  typedef vec<unsigned char, 1>  vec1ub;
  typedef vec<unsigned char, 2>  vec2ub;
  typedef vec<unsigned char, 3>  vec3ub;
  typedef vec<char, 1>           vec1b;
  typedef vec<char, 2>           vec2b;
  typedef vec<char, 3>           vec3b;
  typedef vec<unsigned short, 1> vec1us;
  typedef vec<unsigned short, 2> vec2us;
  typedef vec<unsigned short, 3> vec3us;
  typedef vec<short, 1>          vec1s;
  typedef vec<short, 2>          vec2s;
  typedef vec<short, 3>          vec3s;
  typedef vec<unsigned, 1>       vec1u;
  typedef vec<unsigned, 2>       vec2u;
  typedef vec<unsigned, 3>       vec3u;
  typedef vec<int, 1>            vec1i;
  typedef vec<int, 2>            vec2i;
  typedef vec<int, 3>            vec3i;
  typedef vec<float, 1>          vec1f;
  typedef vec<float, 2>          vec2f;
  typedef vec<float, 3>          vec3f;
  typedef vec<double, 1>         vec1d;
  typedef vec<double, 2>         vec2d;
  typedef vec<double, 3>         vec3d;
} // namespace ano
