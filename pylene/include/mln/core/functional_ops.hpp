#pragma once

#include <mln/core/private/maths_ops.hpp>
#include <mln/core/vec/vec_maths_ops.hpp>

#include <cmath>


#include <mln/core/private/def_functional_ops_gen_code_macros.hpp>

namespace mln
{
  namespace functional
  {
    template <typename T = void>
    struct sqrt_t;
    template <typename T = void>
    struct cbrt_t;
    template <typename T1 = void, typename T2 = T1>
    struct pow_t;
    template <typename T = void>
    struct sqr_t;
    template <typename T = void>
    struct abs_t;
    template <typename T = void>
    struct sum_t;
    template <typename T = void>
    struct prod_t;
    template <typename T1 = void, typename T2 = T1>
    struct dot_t;
    template <typename T1 = void, typename T2 = T1>
    struct cross_t;
    template <typename T = void>
    struct minimum_t;
    template <typename T = void>
    struct maximum_t;

    template <typename T = void>
    struct l0norm_t;
    template <typename T = void>
    struct l1norm_t;
    template <typename T = void>
    struct l2norm_t;
    template <typename T = void>
    struct l2norm_sqr_t;
    template <typename T = void>
    struct linfnorm_t;
    template <unsigned p, typename T = void>
    struct lpnorm_t;

    template <typename T1 = void, typename T2 = T1>
    struct l0dist_t;
    template <typename T1 = void, typename T2 = T1>
    struct l1dist_t;
    template <typename T1 = void, typename T2 = T1>
    struct l2dist_t;
    template <unsigned p, typename T1 = void, typename T2 = T1>
    struct lpdist_t;
    template <typename T1 = void, typename T2 = T1>
    struct l2dist_sqr_t;
    template <typename T1 = void, typename T2 = T1>
    struct linfdist_t;
  }; // namespace functional


  MLN_PRIVATE_FUNCTIONAL_GEN_UNARY_CODE(sqrt);
  MLN_PRIVATE_FUNCTIONAL_GEN_UNARY_CODE(cbrt);
  MLN_PRIVATE_FUNCTIONAL_GEN_BINARY_CODE(pow);
  MLN_PRIVATE_FUNCTIONAL_GEN_UNARY_CODE(sqr);
  MLN_PRIVATE_FUNCTIONAL_GEN_UNARY_CODE(abs);
  MLN_PRIVATE_FUNCTIONAL_GEN_UNARY_CODE(sum);
  MLN_PRIVATE_FUNCTIONAL_GEN_UNARY_CODE(prod);
  MLN_PRIVATE_FUNCTIONAL_GEN_BINARY_CODE(dot);
  MLN_PRIVATE_FUNCTIONAL_GEN_BINARY_CODE(cross);
  MLN_PRIVATE_FUNCTIONAL_GEN_UNARY_CODE(maximum);
  MLN_PRIVATE_FUNCTIONAL_GEN_UNARY_CODE(minimum);

  MLN_PRIVATE_FUNCTIONAL_GEN_UNARY_CODE(l0norm);
  MLN_PRIVATE_FUNCTIONAL_GEN_UNARY_CODE(l1norm);
  MLN_PRIVATE_FUNCTIONAL_GEN_UNARY_CODE(l2norm);
  MLN_PRIVATE_FUNCTIONAL_GEN_UNARY_CODE(l2norm_sqr);
  MLN_PRIVATE_FUNCTIONAL_GEN_UNARY_CODE(linfnorm);
  namespace functional
  {
    template <unsigned p, typename T>
    struct lpnorm_t
    {
      using result_type = decltype(lpnorm<p>(std::declval<T>()));
      auto operator()(const T& x) const { return lpnorm<p>(x); }
    };
    template <unsigned p>
    struct lpnorm_t<p, void>
    {
      template <typename T>
      auto operator()(const T& x) const
      {
        return lpnorm<p>(x);
      }
    };
  } // namespace functional

  MLN_PRIVATE_FUNCTIONAL_GEN_BINARY_CODE(l0dist);
  MLN_PRIVATE_FUNCTIONAL_GEN_BINARY_CODE(l1dist);
  MLN_PRIVATE_FUNCTIONAL_GEN_BINARY_CODE(l2dist);
  MLN_PRIVATE_FUNCTIONAL_GEN_BINARY_CODE(l2dist_sqr);
  MLN_PRIVATE_FUNCTIONAL_GEN_BINARY_CODE(linfdist);
  namespace functional
  {
    template <unsigned p, typename T1, typename T2>
    struct lpdist_t
    {
      using result_type = decltype(lpdist<p>(std::declval<T1>(), std::declval<T2>()));
      auto operator()(const T1& x, const T2& y) const { return lpdist<p>(x, y); }
    };
    template <unsigned p>
    struct lpdist_t<p, void, void>
    {
      template <typename T1, typename T2>
      auto operator()(const T1& x, const T2& y) const
      {
        return lpdist<p>(x, y);
      }
    };
  } // namespace functional
} // namespace mln

#include <mln/core/private/undef_functional_ops_gen_code_macros.hpp>
