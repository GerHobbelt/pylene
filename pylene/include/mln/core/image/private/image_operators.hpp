#pragma once

#include <functional>
#include <mln/core/image/view/transform.hpp>
#include <mln/core/image/view/zip.hpp>
#include <mln/core/rangev3/foreach.hpp>

namespace mln
{


#define MLN_PRIVATE_DEFINE_UNARY_OPERATOR(op, f)                                                                       \
  template <class I>                                                                                                   \
  auto op(const New_Image<I>& ima)                                                                                     \
  {                                                                                                                    \
    return view::transform(static_cast<const I&>(ima), f);                                                             \
  }


#define MLN_PRIVATE_DEFINE_BINARY_OPERATOR(op, f)                                                                      \
  template <class I1, class I2>                                                                                        \
  auto op(const New_Image<I1>& ima1, const New_Image<I2>& ima2)                                                        \
  {                                                                                                                    \
    return view::transform(static_cast<const I1&>(ima1), static_cast<const I2&>(ima2), f);                             \
  }                                                                                                                    \
                                                                                                                       \
  template <class I, class Scalar>                                                                                     \
  auto op(const New_Image<I>& ima1, Scalar s, std::enable_if_t<!is_a<Scalar, New_Image>::value>* = nullptr)            \
  {                                                                                                                    \
    auto g = [f_ = f, s](auto&& arg) { return f_(arg, s); };                                                           \
    return view::transform(static_cast<const I&>(ima1), g);                                                            \
  }                                                                                                                    \
                                                                                                                       \
  template <class Scalar, class I>                                                                                     \
  auto op(Scalar s, const New_Image<I>& ima2, std::enable_if_t<!is_a<Scalar, New_Image>::value>* = nullptr)            \
  {                                                                                                                    \
    auto g = [f_ = f, s](auto&& arg) { return f_(s, arg); };                                                           \
    return view::transform(static_cast<const I&>(ima2), g);                                                            \
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

  MLN_PRIVATE_DEFINE_UNARY_OPERATOR(new_unary_minus, std::negate<>());
  MLN_PRIVATE_DEFINE_UNARY_OPERATOR(new_lnot, std::logical_not<>());


  MLN_PRIVATE_DEFINE_BINARY_OPERATOR(new_eq, std::equal_to<>());
  MLN_PRIVATE_DEFINE_BINARY_OPERATOR(new_neq, std::not_equal_to<>());
  MLN_PRIVATE_DEFINE_BINARY_OPERATOR(new_lt, std::less<>());
  MLN_PRIVATE_DEFINE_BINARY_OPERATOR(new_gt, std::greater<>());
  MLN_PRIVATE_DEFINE_BINARY_OPERATOR(new_lte, std::less_equal<>());
  MLN_PRIVATE_DEFINE_BINARY_OPERATOR(new_gte, std::greater_equal<>());
  MLN_PRIVATE_DEFINE_BINARY_OPERATOR(new_land, std::logical_and<>());
  MLN_PRIVATE_DEFINE_BINARY_OPERATOR(new_lor, std::logical_or<>());


  MLN_PRIVATE_DEFINE_BINARY_OPERATOR(new_plus, details::to_common_type<std::plus<>>());
  MLN_PRIVATE_DEFINE_BINARY_OPERATOR(new_minus, details::to_common_type<std::minus<>>());
  MLN_PRIVATE_DEFINE_BINARY_OPERATOR(new_multiplies, details::to_common_type<std::multiplies<>>());
  MLN_PRIVATE_DEFINE_BINARY_OPERATOR(new_devides, details::to_common_type<std::divides<>>());
  MLN_PRIVATE_DEFINE_BINARY_OPERATOR(new_modulus, details::to_common_type<std::modulus<>>());


  namespace details
  {
    template <class ICond, class ITrue, class IFalse, class = void>
    struct where_fn;


    template <class ICond, class ITrue, class IFalse>
    struct where_fn<ICond, ITrue, IFalse, std::enable_if_t<is_a<ITrue, Image>::value && is_a<IFalse, New_Image>::value>>
    {
      auto operator()(const ICond& cond, ITrue iftrue, IFalse iffalse) const
      {
        auto g = [](auto tuple_ternary_expr) -> decltype(auto) {
          return (std::get<0>(tuple_ternary_expr)) ? std::get<1>(tuple_ternary_expr) : std::get<2>(tuple_ternary_expr);
        };

        return view::transform(view::zip(cond, iftrue, iffalse), g);
      }
    };

    template <class ICond, class ITrue, class IFalse>
    struct where_fn<ICond, ITrue, IFalse,
                    std::enable_if_t<!is_a<ITrue, Image>::value && is_a<IFalse, New_Image>::value>>
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
    struct where_fn<ICond, ITrue, IFalse,
                    std::enable_if_t<is_a<ITrue, Image>::value && !is_a<IFalse, New_Image>::value>>
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
    struct where_fn<ICond, ITrue, IFalse,
                    std::enable_if_t<!is_a<ITrue, Image>::value && !is_a<IFalse, New_Image>::value>>
    {
      auto operator()(const ICond& cond, ITrue vtrue, IFalse vfalse) const
      {
        auto g = [vtrue, vfalse](bool vcond) { return (vcond) ? vtrue : vfalse; };
        return view::transform(cond, g);
      }
    };
  } // namespace details

  template <class ICond, class ITrue, class IFalse>
  auto where(const New_Image<ICond>& cond, ITrue iftrue, IFalse iffalse)
  {
    return details::where_fn<ICond, ITrue, IFalse>()(static_cast<const ICond&>(cond), std::move(iftrue),
                                                     std::move(iffalse));
  }


  // FIXME: deprecated => replace with algorithm all_of
  template <class I>
  bool new_all(I ima)
  {
    static_assert(mln::is_a<I, New_Image>());
    static_assert(std::is_convertible<typename I::reference, bool>());


    mln_foreach_new (auto&& val, ima.new_values())
      if (!val)
        return false;

    return true;
  }


  template <class I>
  bool new_any(I ima)
  {
    static_assert(mln::is_a<I, New_Image>());
    static_assert(std::is_convertible<typename I::reference, bool>());

    mln_foreach_new (auto&& val, ima.new_values())
      if (val)
        return true;

    return false;
  }


#undef MLN_PRIVATE_DEFINE_UNARY_OPERATOR
#undef MLN_PRIVATE_DEFINE_BINARY_OPERATOR
} // namespace mln
