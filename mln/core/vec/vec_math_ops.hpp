#ifndef MLN_CORE_VEC_VEC_MATH_OPS_HPP
# define MLN_CORE_VEC_VEC_MATH_OPS_HPP

# include <mln/core/vec.hpp>
# include <mln/core/math_ops.hpp>


/**
* \file Define vectorial overloads for fundamental mathematical and
* statistical operators
*
*/

# define MLN_VEC_PROMOTE_FUN(T, dim, tag, fun)				\
  internal::vec_base< decltype(fun(std::declval<T>())), dim, tag>

# define MLN_PROMOTE(T)					\
  decltype(std::declval<T>() + std::declval<T>())

# define MLN_PROMOTE_FUN(T, fun)		\
  decltype(fun(std::declval<T>()))

# define MLN_PROMOTE_FUNCOMP(T, f, g)		\
  decltype(f(g(std::declval<T>())))

namespace mln
{

  /* Element wise operators */
  template <class T, unsigned dim, class tag>
  MLN_VEC_PROMOTE_FUN(T, dim, tag, sqr)
    sqr(const internal::vec_base<T, dim, tag>& x);

  template <class T, unsigned dim, class tag>
  MLN_VEC_PROMOTE_FUN(T, dim, tag, abs)
    abs(const internal::vec_base<T, dim, tag>& x);

  template <class T, unsigned dim, class tag>
  MLN_VEC_PROMOTE_FUN(T, dim, tag, sqrt)
    sqrt(const internal::vec_base<T, dim, tag>& x);

  template <class T, unsigned dim, class tag>
  MLN_VEC_PROMOTE_FUN(T, dim, tag, cqrt)
    cbrt(const internal::vec_base<T, dim, tag>& x);


  /* Reduction operators */

  template <typename T, unsigned dim, class tag>
  MLN_PROMOTE(T)
  sum(const internal::vec_base<T, dim, tag>& x);

  template <typename T, unsigned dim, class tag>
  MLN_PROMOTE(T)
  prod(const internal::vec_base<T, dim, tag>& x);

  template <typename T, unsigned dim, class tag>
  T
  maximum(const internal::vec_base<T, dim, tag>& x);

  template <typename T, unsigned dim, class tag>
  T
  minimum(const internal::vec_base<T, dim, tag>& x);


  template <typename T, unsigned dim, class tag>
  MLN_PROMOTE_FUN(T, abs)
    l0norm(const internal::vec_base<T, dim, tag>& x);

  template <typename T, unsigned dim, class tag>
  MLN_PROMOTE_FUN(T, abs)
    linfnorm(const internal::vec_base<T, dim, tag>& x);

  template <typename T, unsigned dim, class tag>
  MLN_PROMOTE_FUN(T, abs)
    l1norm(const internal::vec_base<T, dim, tag>& x);

  template <typename T, unsigned dim, class tag>
  MLN_PROMOTE_FUNCOMP(T, sqrt, sqr)
    l2norm(const internal::vec_base<T, dim, tag>& x);

  template <unsigned p, typename T, unsigned dim, class tag>
  MLN_PROMOTE_FUNCOMP(T, pow, abs)
    lpnorm(const internal::vec_base<T, dim, tag>& x);




  /*****************************/
  /**   Implementation       ***/
  /*****************************/

# define MLN_GEN_CODE(FUN)			\
  template <class T, unsigned dim, class tag>	\
  inline					\
  MLN_VEC_PROMOTE_FUN(T, dim, tag, FUN)		\
    FUN(const internal::vec_base<T, dim, tag>& x)		\
  {						\
    MLN_VEC_PROMOTE_FUN(T, dim, tag, FUN) res;	\
    for (unsigned i = 0; i < dim; ++i)		\
      res[i] = FUN(x[i]);			\
    return res;					\
  }

  MLN_GEN_CODE(sqr);
  MLN_GEN_CODE(sqrt);
  MLN_GEN_CODE(abs);
  MLN_GEN_CODE(cbrt);


# undef MLN_GEN_CODE

  template <typename T, unsigned dim, class tag>
  inline
  MLN_PROMOTE(T)
  sum(const internal::vec_base<T, dim, tag>& x)
  {
    MLN_PROMOTE(T) res = x[0];
    for (unsigned i = 1; i < dim; ++i)
      res[i] += x[i];
    return res;
  }

  template <typename T, unsigned dim, class tag>
  inline
  MLN_PROMOTE(T)
  prod(const internal::vec_base<T, dim, tag>& x)
  {
    MLN_PROMOTE(T) res = x[0];
    for (unsigned i = 1; i < dim; ++i)
      res[i] *= x[i];
    return res;
  }


  template <typename T, unsigned dim, class tag>
  inline
  T
  maximum(const internal::vec_base<T, dim, tag>& x)
  {
    T res = x[0];
    for (unsigned i = 1; i < dim; ++i)
      if (res < x[i])
	res = x[i];
    return res;
  }

  template <typename T, unsigned dim, class tag>
  inline
  T
  minimum(const internal::vec_base<T, dim, tag>& x)
  {
    T res = x[0];
    for (unsigned i = 1; i < dim; ++i)
      if (x[i] < res)
	res = x[i];
    return res;
  }

  template <typename T, unsigned dim, class tag>
  inline
  MLN_PROMOTE_FUN(T, abs)
    l0norm(const internal::vec_base<T, dim, tag>& x)
  {
    typedef MLN_PROMOTE_FUN(T, abs) U;
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
  inline
  MLN_PROMOTE_FUN(T, abs)
    linfnorm(const internal::vec_base<T, dim, tag>& x)
  {
    typedef MLN_PROMOTE_FUN(T, abs) U;
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
  inline
  MLN_PROMOTE_FUN(T, abs)
    l1norm(const internal::vec_base<T, dim, tag>& x)
  {
    typedef MLN_PROMOTE_FUN(T, abs) U;
    U res = abs(x[0]);
    for (unsigned i = 1; i < dim; ++i)
      res += abs(x[i]);
    return res;
  }

  template <typename T, unsigned dim, class tag>
  inline
  MLN_PROMOTE_FUNCOMP(T, sqrt, sqr)
    l2norm(const internal::vec_base<T, dim, tag>& x)
  {
    typedef MLN_PROMOTE_FUNCOMP(T, sqrt, sqr) U;
    U res = sqr(x[0]);
    for (unsigned i = 1; i < dim; ++i)
      res += sqr(x[i]);
    return sqrt(res);
  }


  template <unsigned p, typename T, unsigned dim, class tag>
  MLN_PROMOTE_FUNCOMP(T, pow, abs)
    lpnorm(const internal::vec_base<T, dim, tag>& x)
  {
    typedef MLN_PROMOTE_FUNCOMP(T, pow, abs) U;
    U res = pow(abs(x[0]), p);
    for (unsigned i = 1; i < dim; ++i)
      res += pow(abs(x[i]), p);
    return pow(res, 1/p);
  }

}

# undef MLN_VEC_PROMOTE_FUN
# undef MLN_VEC_PROMOTE

#endif // ! MLN_CORE_VEC_VEC_MATH_OPS_HPP