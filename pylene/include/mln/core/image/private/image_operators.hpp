#pragma once

#include <functional>
#include <mln/core/image/view/transform.hpp>
#include <mln/core/rangev3/foreach.hpp>

namespace mln::experimental
{
  // Prevent ADL on those operators so that A+B uses old API
  // and using mln::experimental::ops A+B uses new API
  // to be reverted when the new API is validated
  namespace ops
  {

#define MLN_PRIVATE_DEFINE_UNARY_OPERATOR(op, f)                                                                       \
  template <class I, class = std::enable_if_t<mln::is_a<I, Image>::value>>                                             \
  auto op(const I& ima)                                                                                                \
  {                                                                                                                    \
    return view::transform(static_cast<const I&>(ima), f);                                                             \
  }


#define MLN_PRIVATE_DEFINE_BINARY_OPERATOR(op, f)                                                                      \
  namespace impl                                                                                                       \
  {                                                                                                                    \
    template <class I1, class I2>                                                                                      \
    auto op(const Image<I1>& ima1, const Image<I2>& ima2)                                                              \
    {                                                                                                                  \
      return view::transform(static_cast<const I1&>(ima1), static_cast<const I2&>(ima2), f);                           \
    }                                                                                                                  \
                                                                                                                       \
    template <class I, class Scalar, class = std::enable_if_t<!is_a<Scalar, Image>::value>>                            \
    auto op(const Image<I>& ima1, Scalar s)                                                                            \
    {                                                                                                                  \
      auto g = [f_ = f, s](auto&& arg) { return f_(arg, s); };                                                         \
      return view::transform(static_cast<const I&>(ima1), g);                                                          \
    }                                                                                                                  \
                                                                                                                       \
    template <class Scalar, class I, class = std::enable_if_t<!is_a<Scalar, Image>::value>>                            \
    auto op(Scalar s, const Image<I>& ima2)                                                                            \
    {                                                                                                                  \
      auto g = [f_ = f, s](auto&& arg) { return f_(s, arg); };                                                         \
      return view::transform(static_cast<const I&>(ima2), g);                                                          \
    }                                                                                                                  \
  }                                                                                                                    \
                                                                                                                       \
  /* This overload is there to be a best match wrt old API impl */                                                     \
  template <class A, class B>                                                                                          \
  auto op(const A& lhs, const B& rhs)                                                                                  \
  {                                                                                                                    \
    return mln::experimental::ops::impl::op(lhs, rhs);                                                                 \
  }


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
    MLN_PRIVATE_DEFINE_UNARY_OPERATOR(lnot, std::logical_not<>());


    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(operator==, std::equal_to<>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(operator!=, std::not_equal_to<>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(operator<, std::less<>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(operator>, std::greater<>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(operator<=, std::less_equal<>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(operator>=, std::greater_equal<>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(land, std::logical_and<>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(lor, std::logical_or<>());


    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(operator+, details::to_common_type<std::plus<>>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(operator-, details::to_common_type<std::minus<>>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(operator*, details::to_common_type<std::multiplies<>>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(operator/, details::to_common_type<std::divides<>>());
    MLN_PRIVATE_DEFINE_BINARY_OPERATOR(operator%, details::to_common_type<std::modulus<>>());

  } // namespace ops


  namespace details
  {
    template <class ICond, class ITrue, class IFalse, class = void>
    struct where_fn;


    template <class ICond, class ITrue, class IFalse>
    struct where_fn<ICond, ITrue, IFalse, std::enable_if_t<is_a<ITrue, Image>::value && is_a<IFalse, Image>::value>>
    {
      auto operator()(const ICond& /*cond*/, ITrue /*iftrue*/, IFalse /*iffalse*/) const
      {
        auto g = [](bool vcond, auto&& vtrue, auto&& vfalse) -> decltype(auto) {
          return (vcond) ? std::forward<decltype(vtrue)>(vtrue) : std::forward<decltype(vfalse)>(vfalse);
        };

        // FIXME: to be implemented with imzip
        return;
      }
    };

    template <class ICond, class ITrue, class IFalse>
    struct where_fn<ICond, ITrue, IFalse, std::enable_if_t<!is_a<ITrue, Image>::value && is_a<IFalse, Image>::value>>
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
    struct where_fn<ICond, ITrue, IFalse, std::enable_if_t<is_a<ITrue, Image>::value && !is_a<IFalse, Image>::value>>
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
    struct where_fn<ICond, ITrue, IFalse, std::enable_if_t<!is_a<ITrue, Image>::value && !is_a<IFalse, Image>::value>>
    {
      auto operator()(const ICond& cond, ITrue vtrue, IFalse vfalse) const
      {
        auto g = [vtrue, vfalse](bool vcond) { return (vcond) ? vtrue : vfalse; };
        return view::transform(cond, g);
      }
    };
  } // namespace details

  template <class ICond, class ITrue, class IFalse>
  auto where(const Image<ICond>& cond, ITrue iftrue, IFalse iffalse)
  {
    return details::where_fn<ICond, ITrue, IFalse>()(static_cast<const ICond&>(cond), std::move(iftrue),
                                                     std::move(iffalse));
  }


  // FIXME: deprecated => replace with algorithm all_of
  template <class I>
  [[deprecated]] bool all(I ima)
  {
    static_assert(mln::is_a<I, Image>());
    static_assert(std::is_convertible<typename I::reference, bool>());


    mln_foreach_new (auto&& val, ima.new_values())
      if (!val)
        return false;

    return true;
  }


  template <class I>
  [[deprecated]] bool any(I ima)
  {
    static_assert(mln::is_a<I, Image>());
    static_assert(std::is_convertible<typename I::reference, bool>());

    mln_foreach_new (auto&& val, ima.new_values())
      if (val)
        return true;

    return false;
  }


#undef MLN_PRIVATE_DEFINE_UNARY_OPERATOR
#undef MLN_PRIVATE_DEFINE_BINARY_OPERATOR

} // namespace mln::experimental
