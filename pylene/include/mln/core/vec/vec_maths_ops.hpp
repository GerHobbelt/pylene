#pragma once

#include <mln/core/private/maths_ops.hpp>
#include <mln/core/vec/vec_ops.hpp>

/**
 * \file Define vectorial overloads for fundamental mathematical and
 * statistical operators
 *
 */

#include <mln/core/vec/private/def_gen_code_macros.hpp>

namespace mln
{
  /* Element wise operators */
  template <class U, class V, unsigned dim, class tag>
  internal::vec_base<decltype(pow(std::declval<U>(), std::declval<V>())), dim, tag>
      pow(const internal::vec_base<U, dim, tag>& x, V exp);

  template <class T, unsigned dim, class tag>
  MLN_VEC_PRIVATE_VEC_PROMOTE_FUN(T, dim, tag, sqr)
  sqr(const internal::vec_base<T, dim, tag>& x);

  template <class T, unsigned dim, class tag>
  MLN_VEC_PRIVATE_VEC_PROMOTE_FUN(T, dim, tag, abs)
  abs(const internal::vec_base<T, dim, tag>& x);

  template <class T, unsigned dim, class tag>
  MLN_VEC_PRIVATE_VEC_PROMOTE_FUN(T, dim, tag, sqrt)
  sqrt(const internal::vec_base<T, dim, tag>& x);

  template <class T, unsigned dim, class tag>
  MLN_VEC_PRIVATE_VEC_PROMOTE_FUN(T, dim, tag, cqrt)
  cbrt(const internal::vec_base<T, dim, tag>& x);

  /* algebraic operators */
  template <class U, class V, unsigned dim, class tag>
  MLN_VEC_PRIVATE_PROMOTE(U, V)
  dot(const internal::vec_base<U, dim, tag>& x, const internal::vec_base<V, dim, tag>& y);

  template <class U, class V, class tag>
  internal::vec_base<MLN_VEC_PRIVATE_PROMOTE(U, V), 3, tag> cross(const internal::vec_base<U, 3, tag>& x,
                                                                  const internal::vec_base<V, 3, tag>& y);
  template <class U, class V, class tag>
  internal::vec_base<MLN_VEC_PRIVATE_PROMOTE(U, V), 7, tag> cross(const internal::vec_base<U, 7, tag>& x,
                                                                  const internal::vec_base<V, 7, tag>& y);
  template <class U, class V, unsigned dim, class tag>
  internal::vec_base<MLN_VEC_PRIVATE_PROMOTE(U, V), dim, tag> cross(const internal::vec_base<U, dim, tag>& x,
                                                                    const internal::vec_base<V, dim, tag>& y);

  /* Reduction operators */

  template <typename T, unsigned dim, class tag>
  MLN_VEC_PRIVATE_PROMOTE(T, T)
  sum(const internal::vec_base<T, dim, tag>& x);

  template <typename T, unsigned dim, class tag>
  MLN_VEC_PRIVATE_PROMOTE(T, T)
  prod(const internal::vec_base<T, dim, tag>& x);

  template <typename T, unsigned dim, class tag>
  T maximum(const internal::vec_base<T, dim, tag>& x);

  template <typename T, unsigned dim, class tag>
  T minimum(const internal::vec_base<T, dim, tag>& x);

  template <typename T, unsigned dim, class tag>
  MLN_VEC_PRIVATE_PROMOTE_FUN(T, abs)
  l0norm(const internal::vec_base<T, dim, tag>& x);

  template <typename T, unsigned dim, class tag>
  MLN_VEC_PRIVATE_PROMOTE_FUN(T, abs)
  linfnorm(const internal::vec_base<T, dim, tag>& x);

  template <typename T, unsigned dim, class tag>
  MLN_VEC_PRIVATE_PROMOTE_FUN(T, abs)
  l1norm(const internal::vec_base<T, dim, tag>& x);

  template <typename T, unsigned dim, class tag>
  MLN_VEC_PRIVATE_PROMOTE_FUNCOMP(T, sqrt, sqr)
  l2norm(const internal::vec_base<T, dim, tag>& x);

  template <typename T, unsigned dim, class tag>
  inline MLN_VEC_PRIVATE_PROMOTE_FUN(T, sqr) l2norm_sqr(const internal::vec_base<T, dim, tag>& x);

  template <unsigned p, typename T, unsigned dim, class tag>
  MLN_VEC_PRIVATE_PROMOTE_FUNCOMP(T, pow, abs)
  lpnorm(const internal::vec_base<T, dim, tag>& x);

  template <typename T, unsigned dim, class tag>
  inline auto l0dist(const internal::vec_base<T, dim, tag>& x, const internal::vec_base<T, dim, tag>& y)
      -> decltype(l0norm(x - y));

  template <typename T, unsigned dim, class tag>
  inline auto l1dist(const internal::vec_base<T, dim, tag>& x, const internal::vec_base<T, dim, tag>& y)
      -> decltype(l1norm(x - y));

  template <typename T, unsigned dim, class tag>
  inline auto l2dist(const internal::vec_base<T, dim, tag>& x, const internal::vec_base<T, dim, tag>& y)
      -> decltype(l2norm(x - y));

  template <unsigned p, typename T, unsigned dim, class tag>
  inline auto lpdist(const internal::vec_base<T, dim, tag>& x, const internal::vec_base<T, dim, tag>& y)
      -> decltype(lpnorm<p>(x - y));

  template <typename T, unsigned dim, class tag>
  inline auto l2dist_sqr(const internal::vec_base<T, dim, tag>& x, const internal::vec_base<T, dim, tag>& y)
      -> decltype(l2norm_sqr(x - y));

  template <typename T, unsigned dim, class tag>
  inline auto linfdist(const internal::vec_base<T, dim, tag>& x, const internal::vec_base<T, dim, tag>& y)
      -> decltype(linfnorm(x - y));

  /*****************************/
  /**   Implementation       ***/
  /*****************************/

  MLN_VEC_PRIVATE_GEN_UNARY_CODE(sqr);
  MLN_VEC_PRIVATE_GEN_UNARY_CODE(sqrt);
  MLN_VEC_PRIVATE_GEN_UNARY_CODE(abs);
  MLN_VEC_PRIVATE_GEN_UNARY_CODE(cbrt);

  MLN_VEC_PRIVATE_GEN_BINARY_CODE(l0dist, l0norm, x - y);
  MLN_VEC_PRIVATE_GEN_BINARY_CODE(l1dist, l1norm, x - y);
  MLN_VEC_PRIVATE_GEN_BINARY_CODE(l2dist, l2norm, x - y);
  MLN_VEC_PRIVATE_GEN_BINARY_CODE(l2dist_sqr, l2norm_sqr, x - y);
  MLN_VEC_PRIVATE_GEN_BINARY_CODE(linfdist, linfnorm, x - y);

  template <unsigned p, class T, unsigned dim, class tag>
  inline auto lpdist(const internal::vec_base<T, dim, tag>& x, const internal::vec_base<T, dim, tag>& y)
      -> decltype(lpnorm<p>(x - y))
  {
    return lpnorm<p>(x - y);
  }


  template <class U, class V, unsigned dim, class tag>
  internal::vec_base<decltype(pow(std::declval<U>(), std::declval<V>())), dim, tag>
      pow(const internal::vec_base<U, dim, tag>& x, V exp)
  {
    typedef decltype(pow(std::declval<U>(), std::declval<V>())) R;

    internal::vec_base<R, dim, tag> res;
    for (unsigned i = 0; i < dim; ++i)
      res[i] = pow(x[i], exp);
    return res;
  }

  template <class U, class V, unsigned dim, class tag>
  MLN_VEC_PRIVATE_PROMOTE(U, V)
  dot(const internal::vec_base<U, dim, tag>& x, const internal::vec_base<V, dim, tag>& y)
  {
    MLN_VEC_PRIVATE_PROMOTE(U, V) res = 0;
    for (unsigned i = 0; i < dim; ++i)
      res += x[i] * y[i];
    return res;
  }

  template <class U, class V, class tag>
  internal::vec_base<MLN_VEC_PRIVATE_PROMOTE(U, V), 3, tag> cross(const internal::vec_base<U, 3, tag>& u,
                                                                  const internal::vec_base<V, 3, tag>& v)
  {
    return {u[1] * v[2] - u[2] * v[1], u[2] * v[0] - u[0] * v[2], u[0] * v[1] - u[1] * v[0]};
  }

  template <class U, class V, class tag>
  internal::vec_base<MLN_VEC_PRIVATE_PROMOTE(U, V), 7, tag> cross(const internal::vec_base<U, 7, tag>& u,
                                                                  const internal::vec_base<V, 7, tag>& v)
  {
    // cf. https://en.wikipedia.org/wiki/Seven-dimensional_cross_product#Coordinate_expressions
    return {u[1] * v[3] - u[3] * v[1] + u[2] * v[6] - u[6] * v[2] + u[4] * v[5] - u[5] * v[4],
            u[2] * v[4] - u[4] * v[2] + u[3] * v[0] - u[0] * v[3] + u[5] * v[6] - u[6] * v[5],
            u[3] * v[5] - u[5] * v[3] + u[4] * v[1] - u[1] * v[4] + u[6] * v[0] - u[0] * v[6],
            u[4] * v[6] - u[6] * v[4] + u[5] * v[2] - u[2] * v[5] + u[0] * v[1] - u[1] * v[0],
            u[5] * v[0] - u[0] * v[5] + u[6] * v[3] - u[3] * v[6] + u[1] * v[2] - u[2] * v[1],
            u[6] * v[1] - u[1] * v[6] + u[0] * v[4] - u[4] * v[0] + u[2] * v[3] - u[3] * v[2],
            u[0] * v[2] - u[2] * v[0] + u[1] * v[5] - u[5] * v[1] + u[3] * v[4] - u[4] * v[3]};
  }

  template <class U, class V, unsigned dim, class tag>
  internal::vec_base<MLN_VEC_PRIVATE_PROMOTE(U, V), dim, tag>
      cross([[maybe_unused]] const internal::vec_base<U, dim, tag>& u,
            [[maybe_unused]] const internal::vec_base<V, dim, tag>& v)
  {
    static_assert(dim != 3 && dim != 7, "cross product is only defined for vector of size 3 or 7");
  }

  template <typename T, unsigned dim, class tag>
  inline MLN_VEC_PRIVATE_PROMOTE(T, T) sum(const internal::vec_base<T, dim, tag>& x)
  {
    MLN_VEC_PRIVATE_PROMOTE(T, T) res = x[0];
    for (unsigned i = 1; i < dim; ++i)
      res += x[i];
    return res;
  }

  template <typename T, unsigned dim, class tag>
  inline MLN_VEC_PRIVATE_PROMOTE(T, T) prod(const internal::vec_base<T, dim, tag>& x)
  {
    MLN_VEC_PRIVATE_PROMOTE(T, T) res = x[0];
    for (unsigned i = 1; i < dim; ++i)
      res *= x[i];
    return res;
  }

  template <typename T, unsigned dim, class tag>
  inline T maximum(const internal::vec_base<T, dim, tag>& x)
  {
    T res = x[0];
    for (unsigned i = 1; i < dim; ++i)
      if (res < x[i])
        res = x[i];
    return res;
  }

  template <typename T, unsigned dim, class tag>
  inline T minimum(const internal::vec_base<T, dim, tag>& x)
  {
    T res = x[0];
    for (unsigned i = 1; i < dim; ++i)
      if (x[i] < res)
        res = x[i];
    return res;
  }

  template <typename T, unsigned dim, class tag>
  inline MLN_VEC_PRIVATE_PROMOTE_FUN(T, abs) l0norm(const internal::vec_base<T, dim, tag>& x)
  {
    typedef MLN_VEC_PRIVATE_PROMOTE_FUN(T, abs) U;
    U res = abs(x[0]);
    for (unsigned i = 1; i < dim; ++i)
    {
      U v = abs(x[i]);
      if (v < res)
        res = v;
    }
    return res;
  }

  template <typename T, unsigned dim, class tag>
  inline MLN_VEC_PRIVATE_PROMOTE_FUN(T, abs) linfnorm(const internal::vec_base<T, dim, tag>& x)
  {
    typedef MLN_VEC_PRIVATE_PROMOTE_FUN(T, abs) U;
    U res = abs(x[0]);
    for (unsigned i = 1; i < dim; ++i)
    {
      U v = abs(x[i]);
      if (res < v)
        res = v;
    }
    return res;
  }

  template <typename T, unsigned dim, class tag>
  inline MLN_VEC_PRIVATE_PROMOTE_FUN(T, abs) l1norm(const internal::vec_base<T, dim, tag>& x)
  {
    typedef MLN_VEC_PRIVATE_PROMOTE_FUN(T, abs) U;
    U res = abs(x[0]);
    for (unsigned i = 1; i < dim; ++i)
      res += abs(x[i]);
    return res;
  }

  template <typename T, unsigned dim, class tag>
  inline MLN_VEC_PRIVATE_PROMOTE_FUNCOMP(T, sqrt, sqr) l2norm(const internal::vec_base<T, dim, tag>& x)
  {
    typedef MLN_VEC_PRIVATE_PROMOTE_FUNCOMP(T, sqrt, sqr) U;
    U res = sqr(x[0]);
    for (unsigned i = 1; i < dim; ++i)
      res += sqr(x[i]);
    return sqrt(res);
  }

  template <typename T, unsigned dim, class tag>
  inline MLN_VEC_PRIVATE_PROMOTE_FUN(T, sqr) l2norm_sqr(const internal::vec_base<T, dim, tag>& x)
  {
    typedef MLN_VEC_PRIVATE_PROMOTE_FUNCOMP(T, sqrt, sqr) U;
    U res = sqr(x[0]);
    for (unsigned i = 1; i < dim; ++i)
      res += sqr(x[i]);
    return res;
  }

  template <unsigned p, typename T, unsigned dim, class tag>
  auto lpnorm(const internal::vec_base<T, dim, tag>& x)
  {
    double res = pow(abs(x[0]), p);
    for (unsigned i = 1; i < dim; ++i)
      res += pow(abs(x[i]), p);
    return pow(res, 1. / static_cast<double>(p));
  }
} // namespace mln

#include <mln/core/vec/private/undef_gen_code_macros.hpp>
