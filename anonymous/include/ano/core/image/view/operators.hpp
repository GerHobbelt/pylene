#pragma once

#include <ano/core/image/image.hpp>
#include <ano/core/image/view/transform.hpp>
#include <ano/core/image/view/zip.hpp>

#include <ano/core/vec_base.hpp>

#include <functional>
#include <type_traits>
#include <utility>

#include <ano/core/image/private/def_gen_code_macros.hpp>

namespace ano::view
{
  // Prevent ADL on those operators so that A+B uses old API
  // and using ano::view::ops A+B uses new API
  // to be reverted when the new API is validated
  namespace ops
  {

    namespace details
    {
      template <class Base>
      struct to_common_type : Base
      {
        template <class U, class V>
        std::common_type_t<U, V> operator()(U&& x, V&& y) const
        {
          return static_cast<const Base&>(*this)(std::forward<U>(x), std::forward<V>(y));
        }
      };
    } // namespace details

    ANO_PRIVATE_DEFINE_UNARY_OPERATOR(operator-, std::negate<>());
    ANO_PRIVATE_DEFINE_UNARY_OPERATOR(operator!, std::logical_not<>());

    ANO_PRIVATE_DEFINE_BINARY_OPERATOR(operator==, std::equal_to<>());
    ANO_PRIVATE_DEFINE_BINARY_OPERATOR(operator!=, std::not_equal_to<>());
    ANO_PRIVATE_DEFINE_BINARY_OPERATOR(operator<, std::less<>());
    ANO_PRIVATE_DEFINE_BINARY_OPERATOR(operator>, std::greater<>());
    ANO_PRIVATE_DEFINE_BINARY_OPERATOR(operator<=, std::less_equal<>());
    ANO_PRIVATE_DEFINE_BINARY_OPERATOR(operator>=, std::greater_equal<>());
    ANO_PRIVATE_DEFINE_BINARY_OPERATOR(operator&&, std::logical_and<>());
    ANO_PRIVATE_DEFINE_BINARY_OPERATOR(operator||, std::logical_or<>());


    ANO_PRIVATE_DEFINE_BINARY_OPERATOR(operator+, details::to_common_type<std::plus<>>());
    ANO_PRIVATE_DEFINE_BINARY_OPERATOR(operator-, details::to_common_type<std::minus<>>());
    ANO_PRIVATE_DEFINE_BINARY_OPERATOR(operator*, details::to_common_type<std::multiplies<>>());
    ANO_PRIVATE_DEFINE_BINARY_OPERATOR(operator/, details::to_common_type<std::divides<>>());
    ANO_PRIVATE_DEFINE_BINARY_OPERATOR(operator%, details::to_common_type<std::modulus<>>());

    // FIXME: integrate a transient mechanism to do this comparison : implement operators on vec_base
    namespace impl
    {
      struct equalFP_t
      {
        equalFP_t(double eps)
          : eps_(eps)
        {
        }

        bool operator()(double l, double r) const { return std::abs(l - r) < eps_; }

        template <unsigned dim, class tag>
        bool operator()(const ano::internal::vec_base<double, dim, tag>& l,
                        const ano::internal::vec_base<double, dim, tag>& r) const
        {
          for (unsigned i = 0; i < dim; ++i)
            if (std::abs(l[i] - r[i]) >= eps_)
              return false;
          return true;
        }

      private:
        double eps_;
      };

      template <class I1, class I2>
      auto equalFP(const ::ano::details::Image<I1>& ima1, const ::ano::details::Image<I2>& ima2, double eps)
      {
        return ::ano::view::transform(static_cast<const I1&>(ima1), static_cast<const I2&>(ima2), equalFP_t{eps});
      }

      template <class I, class Scalar, class = std::enable_if_t<!::ano::is_a<Scalar, ::ano::details::Image>::value>>
      auto equalFP(const ::ano::details::Image<I>& ima1, Scalar s, double eps)
      {
        auto g = [f_ = equalFP_t{eps}, s_ = s](auto&& arg) { return f_(arg, s_); };
        return ::ano::view::transform(static_cast<const I&>(ima1), g);
      }

      template <class Scalar, class I, class = std::enable_if_t<!::ano::is_a<Scalar, ::ano::details::Image>::value>>
      auto equalFP(Scalar s, const ::ano::details::Image<I>& ima2, double eps)
      {
        auto g = [f_ = equalFP_t{eps}, s_ = s](auto&& arg) { return f_(s_, arg); };
        return ::ano::view::transform(static_cast<const I&>(ima2), g);
      }
    } /* namespace impl */

    /* This overload is there to be a best match wrt old API impl */
    template <class A, class B,
              class = std::enable_if_t<(::ano::is_a<A, ::ano::details::Image>::value ||
                                        ::ano::is_a<B, ::ano::details::Image>::value)>>
    auto equalFP(const A& lhs, const B& rhs, double eps)
    {
      return impl::equalFP(lhs, rhs, eps);
    }
  } // namespace ops


  namespace details
  {
    template <class ICond, class ITrue, class IFalse, class = void>
    struct ifelse_fn;


    template <class ICond, class ITrue, class IFalse>
    struct ifelse_fn<
        ICond, ITrue, IFalse,
        std::enable_if_t<(is_a<ITrue, ano::details::Image>::value && is_a<IFalse, ano::details::Image>::value)>>
    {
      auto operator()(const ICond& cond, ITrue iftrue, IFalse iffalse) const
      {
        auto g = [](auto&& tuple_ternary_expr) -> decltype(auto) {
          auto&& [im_c, im_t, im_f] = std::forward<decltype(tuple_ternary_expr)>(tuple_ternary_expr);
          return im_c ? im_t : im_f;
        };

        return view::transform(view::zip(cond, iftrue, iffalse), g);
      }
    };

    template <class ICond, class ITrue, class IFalse>
    struct ifelse_fn<
        ICond, ITrue, IFalse,
        std::enable_if_t<(!is_a<ITrue, ano::details::Image>::value && is_a<IFalse, ano::details::Image>::value)>>
    {
      auto operator()(const ICond& cond, ITrue vtrue, IFalse iffalse) const
      {
        auto g = [vtrue](bool vcond, auto&& vfalse) {
          return (vcond) ? (vtrue) : std::forward<decltype(vfalse)>(vfalse);
        };

        return view::transform(cond, std::move(iffalse), g);
      }
    };

    template <class ICond, class ITrue, class IFalse>
    struct ifelse_fn<
        ICond, ITrue, IFalse,
        std::enable_if_t<(is_a<ITrue, ano::details::Image>::value && !is_a<IFalse, ano::details::Image>::value)>>
    {
      auto operator()(const ICond& cond, ITrue iftrue, IFalse vfalse) const
      {
        auto g = [vfalse](bool vcond, auto&& vtrue) {
          return (vcond) ? std::forward<decltype(vtrue)>(vtrue) : (vfalse);
        };

        return view::transform(cond, std::move(iftrue), g);
      }
    };


    template <class ICond, class ITrue, class IFalse>
    struct ifelse_fn<
        ICond, ITrue, IFalse,
        std::enable_if_t<(!is_a<ITrue, ano::details::Image>::value && !is_a<IFalse, ano::details::Image>::value)>>
    {
      auto operator()(const ICond& cond, ITrue vtrue, IFalse vfalse) const
      {
        auto g = [vtrue, vfalse](bool vcond) { return (vcond) ? vtrue : vfalse; };
        return view::transform(cond, g);
      }
    };
  } // namespace details

  template <class ICond, class ITrue, class IFalse>
  auto ifelse(const ano::details::Image<ICond>& cond, ITrue iftrue, IFalse iffalse)
  {
    return details::ifelse_fn<ICond, ITrue, IFalse>()(static_cast<const ICond&>(cond), std::move(iftrue),
                                                      std::move(iffalse));
  }

#include <ano/core/image/private/undef_gen_code_macros.hpp>

} // namespace ano::view
