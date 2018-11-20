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
    return view::transform(static_cast<const I&>(ima), f);            \
  }













  // MLN_PRIVATE_DEFINE_UNARY_OPERATOR(operator-, std::negate<>());
  // MLN_PRIVATE_DEFINE_UNARY_OPERATOR(lnot, std::logical_not<>());

  MLN_PRIVATE_DEFINE_UNARY_OPERATOR(new_unary_minus, std::negate<>());
  MLN_PRIVATE_DEFINE_UNARY_OPERATOR(new_lnot, std::logical_not<>());


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



#undef PRIVATE_DEFINE_UNARY_OPERATOR
}
