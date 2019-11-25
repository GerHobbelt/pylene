#ifndef MLN_CORE_VEC_VEC_MATH_OPS_HPP
#define MLN_CORE_VEC_VEC_MATH_OPS_HPP

#include <mln/core/math_ops.hpp>
#include <mln/core/vec.hpp>

/**
 * \file Define vectorial overloads for fundamental mathematical and
 * statistical operators
 *
 */

namespace mln
{
  namespace details
  {
    template <class U, class V = U>
    using _promote_t = decltype(std::declval<U>() + std::declval<V>());
  }

  /* Element wise operators */
  template <class U, class V, unsigned dim, class tag>
  internal::vec_base<decltype(pow(std::declval<U>(), std::declval<V>())), dim, tag>
  pow(const internal::vec_base<U, dim, tag>& x, V exp);

  template <class T, unsigned dim, class tag>
  auto sqr(const internal::vec_base<T, dim, tag>& x);

  template <class T, unsigned dim, class tag>
  auto abs(const internal::vec_base<T, dim, tag>& x);

  template <class T, unsigned dim, class tag>
  auto sqrt(const internal::vec_base<T, dim, tag>& x);

  template <class T, unsigned dim, class tag>
  auto cbrt(const internal::vec_base<T, dim, tag>& x);

  /* algebraic operators */
  template <class U, class V, unsigned dim, class tag>
  auto dot(const internal::vec_base<U, dim, tag>& x, const internal::vec_base<V, dim, tag>& y)
      -> details::_promote_t<U, V>;

  template <class U, class V, class tag>
  auto cross(const internal::vec_base<U, 3, tag>& x, const internal::vec_base<V, 3, tag>& y)
      -> internal::vec_base<details::_promote_t<U, V>, 3, tag>;

  /* Reduction operators */

  template <typename T, unsigned dim, class tag>
  auto sum(const internal::vec_base<T, dim, tag>& x) -> details::_promote_t<T>;

  template <typename T, unsigned dim, class tag>
  auto prod(const internal::vec_base<T, dim, tag>& x) -> details::_promote_t<T>;

  template <typename T, unsigned dim, class tag>
  T maximum(const internal::vec_base<T, dim, tag>& x);

  template <typename T, unsigned dim, class tag>
  T minimum(const internal::vec_base<T, dim, tag>& x);

  template <typename T, unsigned dim, class tag>
  auto l0norm(const internal::vec_base<T, dim, tag>& x);

  template <typename T, unsigned dim, class tag>
  auto linfnorm(const internal::vec_base<T, dim, tag>& x);

  template <typename T, unsigned dim, class tag>
  auto l1norm(const internal::vec_base<T, dim, tag>& x);

  template <typename T, unsigned dim, class tag>
  auto l2norm(const internal::vec_base<T, dim, tag>& x);

  template <typename T, unsigned dim, class tag>
  auto l2norm_sqr(const internal::vec_base<T, dim, tag>& x);

  template <unsigned p, typename T, unsigned dim, class tag>
  auto lpnorm(const internal::vec_base<T, dim, tag>& x);

  template <typename T, unsigned dim, class tag>
  auto l1dist(const internal::vec_base<T, dim, tag>& x, const internal::vec_base<T, dim, tag>& y);

  template <typename T, unsigned dim, class tag>
  auto l2dist(const internal::vec_base<T, dim, tag>& x, const internal::vec_base<T, dim, tag>& y);

  template <typename T, unsigned dim, class tag>
  auto l2dist_sqr(const internal::vec_base<T, dim, tag>& x, const internal::vec_base<T, dim, tag>& y);

/*****************************/
/**   Implementation       ***/
/*****************************/

#define MLN_GEN_CODE(FUN)                                                                                              \
  template <class T, unsigned dim, class tag>                                                                          \
  auto FUN(const internal::vec_base<T, dim, tag>& x)                                                                   \
  {                                                                                                                    \
    using R = decltype(FUN(std::declval<T>()));                                                                        \
    internal::vec_base<R, dim, tag> res;                                                                               \
    for (unsigned i = 0; i < dim; ++i)                                                                                 \
      res[i] = FUN(x[i]);                                                                                              \
    return res;                                                                                                        \
  }

#define MLN_GEN_BINARY_CODE(FUN, FUNBASE, EXPR)                                                                        \
  template <class T, unsigned dim, class tag>                                                                          \
  auto FUN(const internal::vec_base<T, dim, tag>& x, const internal::vec_base<T, dim, tag>& y)                         \
  {                                                                                                                    \
    return FUNBASE(EXPR);                                                                                              \
  }

  MLN_GEN_CODE(sqr);
  MLN_GEN_CODE(sqrt);
  MLN_GEN_CODE(abs);
  MLN_GEN_CODE(cbrt);

  MLN_GEN_BINARY_CODE(l1dist, l1norm, x - y);
  MLN_GEN_BINARY_CODE(l2dist, l2norm, x - y);
  MLN_GEN_BINARY_CODE(l2dist_sqr, l2norm_sqr, x - y);

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
  auto dot(const internal::vec_base<U, dim, tag>& x, const internal::vec_base<V, dim, tag>& y)
    -> details::_promote_t<U, V>
  {
    details::_promote_t<U, V> res = 0;
    for (int i = 0; i < dim; ++i)
      res += x[i] * y[i];
    return res;
  }

  template <class U, class V, class tag>
  auto cross(const internal::vec_base<U, 3, tag>& u, const internal::vec_base<V, 3, tag>& v)
      -> internal::vec_base<details::_promote_t<U, V>, 3, tag>
  {
    return {u[1] * v[2] - u[2] * v[1], u[2] * v[0] - u[0] * v[2], u[0] * v[1] - u[1] * v[0]};
  }

  template <typename T, unsigned dim, class tag>
  auto sum(const internal::vec_base<T, dim, tag>& x) -> details::_promote_t<T>
  {
    details::_promote_t<T> res = x[0];
    for (unsigned i = 1; i < dim; ++i)
      res += x[i];
    return res;
  }

  template <typename T, unsigned dim, class tag>
  auto prod(const internal::vec_base<T, dim, tag>& x) -> details::_promote_t<T> 
  {
    details::_promote_t<T> res = x[0];
    for (unsigned i = 1; i < dim; ++i)
      res *= x[i];
    return res;
  }

  template <typename T, unsigned dim, class tag>
  T maximum(const internal::vec_base<T, dim, tag>& x)
  {
    T res = x[0];
    for (unsigned i = 1; i < dim; ++i)
      if (res < x[i])
        res = x[i];
    return res;
  }

  template <typename T, unsigned dim, class tag>
  T minimum(const internal::vec_base<T, dim, tag>& x)
  {
    T res = x[0];
    for (unsigned i = 1; i < dim; ++i)
      if (x[i] < res)
        res = x[i];
    return res;
  }

  template <typename T, unsigned dim, class tag>
  auto l0norm(const internal::vec_base<T, dim, tag>& x)
  {
    auto res = abs(x[0]);
    for (unsigned i = 1; i < dim; ++i)
    {
      auto v = abs(x[i]);
      if (v < res)
        res = v;
    }
    return res;
  }

  template <typename T, unsigned dim, class tag>
  auto linfnorm(const internal::vec_base<T, dim, tag>& x)
  {
    auto res = abs(x[0]);
    for (unsigned i = 1; i < dim; ++i)
    {
      auto v = abs(x[i]);
      if (res < v)
        res = v;
    }
    return res;
  }

  template <typename T, unsigned dim, class tag>
  auto l1norm(const internal::vec_base<T, dim, tag>& x)
  {
    auto res = abs(x[0]);
    for (unsigned i = 1; i < dim; ++i)
      res += abs(x[i]);
    return res;
  }

  template <typename T, unsigned dim, class tag>
  auto l2norm(const internal::vec_base<T, dim, tag>& x)
  {
    auto res = sqr(x[0]);
    for (unsigned i = 1; i < dim; ++i)
      res += sqr(x[i]);
    return sqrt(res);
  }

  template <typename T, unsigned dim, class tag>
  auto l2norm_sqr(const internal::vec_base<T, dim, tag>& x)
  {
    auto res = sqr(x[0]);
    for (unsigned i = 1; i < dim; ++i)
      res += sqr(x[i]);
    return res;
  }

  template <unsigned p, typename T, unsigned dim, class tag>
  auto lpnorm(const internal::vec_base<T, dim, tag>& x)
  {
    auto res = pow(abs(x[0]), p);
    for (unsigned i = 1; i < dim; ++i)
      res += pow(abs(x[i]), p);
    return pow(res, 1 / p);
  }
}

#undef MLN_GEN_CODE
#undef MLN_GEN_BINARY_CODE

#endif // ! MLN_CORE_VEC_VEC_MATH_OPS_HPP
