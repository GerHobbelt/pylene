#pragma once

#include <mln/core/image/image.hpp>
#include <mln/core/image/view/transform.hpp>
#include <mln/core/image/view/zip.hpp>

#include <mln/core/vec_base.hpp>

#include <functional>
#include <type_traits>
#include <utility>

#include <mln/core/image/private/def_gen_code_macros.hpp>

namespace mln::view
{
  // Prevent ADL on those operators so that A+B uses old API
  // and using mln::view::ops A+B uses new API
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

    MLN_PRIVATE_DEFINE_UNARY_OPERATOR(operator-, std::negate<>());
    MLN_PRIVATE_DEFINE_UNARY_OPERATOR(operator!, std::logical_not<>());

    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(operator==, std::equal_to<>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(operator!=, std::not_equal_to<>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(operator<, std::less<>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(operator>, std::greater<>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(operator<=, std::less_equal<>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(operator>=, std::greater_equal<>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(operator&&, std::logical_and<>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(operator||, std::logical_or<>());


    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(operator+, details::to_common_type<std::plus<>>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(operator-, details::to_common_type<std::minus<>>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(operator*, details::to_common_type<std::multiplies<>>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(operator/, details::to_common_type<std::divides<>>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(operator%, details::to_common_type<std::modulus<>>());

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
        bool operator()(const mln::internal::vec_base<double, dim, tag>& l,
                        const mln::internal::vec_base<double, dim, tag>& r) const
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
      auto equalFP(const ::mln::details::Image<I1>& ima1, const ::mln::details::Image<I2>& ima2, double eps)
      {
        return ::mln::view::transform(static_cast<const I1&>(ima1), static_cast<const I2&>(ima2), equalFP_t{eps});
      }

      template <class I, class Scalar>
      auto equalFP(const ::mln::details::Image<I>& ima1, Scalar s,
                   double eps) requires(not mln::is_a<Scalar, ::mln::details::Image>::value)
      {
        auto g = [f_ = equalFP_t{eps}, s_ = s](auto&& arg) { return f_(arg, s_); };
        return ::mln::view::transform(static_cast<const I&>(ima1), g);
      }

      template <class Scalar, class I>
      auto equalFP(Scalar s, const ::mln::details::Image<I>& ima2,
                   double eps) requires(not mln::is_a<Scalar, ::mln::details::Image>::value)
      {
        auto g = [f_ = equalFP_t{eps}, s_ = s](auto&& arg) { return f_(s_, arg); };
        return ::mln::view::transform(static_cast<const I&>(ima2), g);
      }
    } /* namespace impl */

    /* This overload is there to be a best match wrt old API impl */
    template <class A, class B>
    auto equalFP(const A& lhs, const B& rhs, double eps) requires(mln::is_a<A, ::mln::details::Image>::value ||
                                                                  mln::is_a<B, ::mln::details::Image>::value)
    {
      return impl::equalFP(lhs, rhs, eps);
    }
  } // namespace ops


  namespace details
  {
    template <class ICond, class ITrue, class IFalse>
    struct ifelse_fn;

    template <class ICond, class ITrue, class IFalse>
    requires(is_a<ITrue, mln::details::Image>::value&& is_a<IFalse, mln::details::Image>::value) //
        struct ifelse_fn<ICond, ITrue, IFalse>
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
    requires(not is_a<ITrue, mln::details::Image>::value && not is_a<IFalse, mln::details::Image>::value) //
        struct ifelse_fn<ICond, ITrue, IFalse>
    {
      auto operator()(const ICond& cond, ITrue vtrue, IFalse vfalse) const
      {
        auto g = [vtrue, vfalse](bool vcond) { return (vcond) ? vtrue : vfalse; };
        return view::transform(cond, g);
      }
    };

    template <class ICond, class ITrue, class IFalse>
    requires(not is_a<ITrue, mln::details::Image>::value && is_a<IFalse, mln::details::Image>::value) //
        struct ifelse_fn<ICond, ITrue, IFalse>
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
    requires(is_a<ITrue, mln::details::Image>::value && not is_a<IFalse, mln::details::Image>::value) //
        struct ifelse_fn<ICond, ITrue, IFalse>
    {
      auto operator()(const ICond& cond, ITrue iftrue, IFalse vfalse) const
      {
        auto g = [vfalse](bool vcond, auto&& vtrue) {
          return (vcond) ? std::forward<decltype(vtrue)>(vtrue) : (vfalse);
        };

        return view::transform(cond, std::move(iftrue), g);
      }
    };
  } // namespace details

  template <class ICond, class ITrue, class IFalse>
  auto ifelse(const mln::details::Image<ICond>& cond, ITrue iftrue, IFalse iffalse)
  {
    return details::ifelse_fn<ICond, ITrue, IFalse>()(static_cast<const ICond&>(cond), std::move(iftrue),
                                                      std::move(iffalse));
  }

#include <mln/core/image/private/undef_gen_code_macros.hpp>

} // namespace mln::view
