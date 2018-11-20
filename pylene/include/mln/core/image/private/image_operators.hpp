#pragma once

#include <mln/core/image/view/transform.hpp>
#include <mln/core/rangev3/foreach.hpp>
#include <functional>

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
        return static_cast<const Base&>(*this) (std::forward<U>(x), std::forward<V>(y));
      }
    };
  }

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


  // FIXME: deprecated => replace with algorithm all_of
  template <class I>
  bool new_all(I ima)
  {
    static_assert(mln::is_a<I, New_Image>());
    static_assert(std::is_convertible<typename I::reference, bool>());


    mln_foreach_new(auto&& val, ima.new_values())
      if (!val)
        return false;

    return true;
  }


  template <class I>
  bool new_any(I ima)
  {
    static_assert(mln::is_a<I, New_Image>());
    static_assert(std::is_convertible<typename I::reference, bool>());

    mln_foreach_new(auto&& val, ima.new_values())
      if (val)
        return true;

    return false;
  }



#undef MLN_PRIVATE_DEFINE_UNARY_OPERATOR
#undef MLN_PRIVATE_DEFINE_BINARY_OPERATOR
}
