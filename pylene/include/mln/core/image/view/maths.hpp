#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/image/private/def_gen_code_macros.hpp>

#include <mln/core/functional_ops.hpp>

namespace mln::view
{

  namespace maths
  {
    MLN_PRIVATE_DEFINE_UNARY_OPERATOR(abs, functional::abs_t<>());
    MLN_PRIVATE_DEFINE_UNARY_OPERATOR(sqr, functional::sqr_t<>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(pow, functional::pow_t<>());
    MLN_PRIVATE_DEFINE_UNARY_OPERATOR(sqrt, functional::sqrt_t<>());
    MLN_PRIVATE_DEFINE_UNARY_OPERATOR(cbrt, functional::cbrt_t<>());
    MLN_PRIVATE_DEFINE_UNARY_OPERATOR(sum, functional::sum_t<>());
    MLN_PRIVATE_DEFINE_UNARY_OPERATOR(prod, functional::prod_t<>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(dot, functional::dot_t<>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(cross, functional::cross_t<>());
    MLN_PRIVATE_DEFINE_UNARY_OPERATOR(min, functional::minimum_t<>());
    MLN_PRIVATE_DEFINE_UNARY_OPERATOR(max, functional::maximum_t<>());

    MLN_PRIVATE_DEFINE_UNARY_OPERATOR(l0norm, functional::l0norm_t<>());
    MLN_PRIVATE_DEFINE_UNARY_OPERATOR(l1norm, functional::l1norm_t<>());
    MLN_PRIVATE_DEFINE_UNARY_OPERATOR(l2norm, functional::l2norm_t<>());
    MLN_PRIVATE_DEFINE_UNARY_OPERATOR(l2norm_sqr, functional::l2norm_sqr_t<>());
    MLN_PRIVATE_DEFINE_UNARY_OPERATOR(linfnorm, functional::linfnorm_t<>());
    template <unsigned p, class I, class = std::enable_if_t<::mln::is_a<I, ::mln::details::Image>::value>>
    auto lpnorm(const I& ima)
    {
      return ::mln::view::transform(static_cast<const I&>(ima), functional::lpnorm_t<p>{});
    }

    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(l0dist, functional::l0dist_t<>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(l1dist, functional::l1dist_t<>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(l2dist, functional::l2dist_t<>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(l2dist_sqr, functional::l2dist_sqr_t<>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(linfdist, functional::linfdist_t<>());
    namespace impl
    {
      template <unsigned p, class I1, class I2>
      auto lpdist(const ::mln::details::Image<I1>& ima1, const ::mln::details::Image<I2>& ima2)
      {
        return ::mln::view::transform(static_cast<const I1&>(ima1), static_cast<const I2&>(ima2),
                                      functional::lpdist_t<p>{});
      }

      template <unsigned p, class I, class Scalar,
                class = std::enable_if_t<!::mln::is_a<Scalar, ::mln::details::Image>::value>>
      auto lpdist(const ::mln::details::Image<I>& ima1, Scalar s)
      {
        auto g = [f_ = functional::lpdist_t<p>{}, s_ = s](auto&& arg) { return f_(arg, s_); };
        return ::mln::view::transform(static_cast<const I&>(ima1), g);
      }

      template <unsigned p, class Scalar, class I,
                class = std::enable_if_t<!::mln::is_a<Scalar, ::mln::details::Image>::value>>
      auto lpdist(Scalar s, const ::mln::details::Image<I>& ima2)
      {
        auto g = [f_ = functional::lpdist_t<p>{}, s_ = s](auto&& arg) { return f_(s_, arg); };
        return ::mln::view::transform(static_cast<const I&>(ima2), g);
      }
    } /* namespace impl */

    /* This overload is there to be a best match wrt old API impl */
    template <unsigned p, class A, class B,
              class = std::enable_if_t<(::mln::is_a<A, ::mln::details::Image>::value ||
                                        ::mln::is_a<B, ::mln::details::Image>::value)>>
    auto lpdist(const A& lhs, const B& rhs)
    {
      return impl::lpdist<p>(lhs, rhs);
    }

  } // namespace maths
} // namespace mln::view

#include <mln/core/image/private/undef_gen_code_macros.hpp>
